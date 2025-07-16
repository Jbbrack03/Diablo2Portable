#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/egl_context.h"
#include "rendering/texture_manager.h"
#include "rendering/vertex_buffer_pool.h"
#include "tools/texture_atlas_generator.h"
#include <glm/vec2.hpp>

namespace d2::rendering {

class SpriteRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        texture_manager = std::make_unique<TextureManager>();
        sprite_renderer = std::make_unique<SpriteRenderer>();
    }

    void TearDown() override {
        sprite_renderer.reset();
        texture_manager.reset();
        renderer.reset();
        context.reset();
    }

    std::unique_ptr<EGLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> texture_manager;
    std::unique_ptr<SpriteRenderer> sprite_renderer;
};

// GREEN PHASE: This test now passes - testing single sprite rendering
TEST_F(SpriteRendererTest, RenderSingleSprite) {
    EXPECT_TRUE(sprite_renderer->initialize(*renderer, *texture_manager));
    
    // Create a test texture ID (simulating uploaded sprite)
    uint32_t texture_id = 1;
    glm::vec2 position(100.0f, 100.0f);
    glm::vec2 size(64.0f, 64.0f);
    
    sprite_renderer->beginFrame();
    sprite_renderer->drawSprite(texture_id, position, size);
    sprite_renderer->endFrame();
    
    EXPECT_EQ(sprite_renderer->getDrawCallCount(), 1u);
    EXPECT_EQ(sprite_renderer->getSpriteCount(), 1u);
}

// RED PHASE: This test MUST fail - testing sprite batching with different textures
TEST_F(SpriteRendererTest, BatchMultipleSprites) {
    EXPECT_TRUE(sprite_renderer->initialize(*renderer, *texture_manager));
    
    sprite_renderer->beginFrame();
    
    // Draw 5 sprites with texture 1
    for (int i = 0; i < 5; i++) {
        sprite_renderer->drawSprite(1, glm::vec2(i * 64.0f, 0.0f), glm::vec2(64.0f, 64.0f));
    }
    
    // Draw 5 sprites with texture 2 (different texture = new batch)
    for (int i = 0; i < 5; i++) {
        sprite_renderer->drawSprite(2, glm::vec2(i * 64.0f, 64.0f), glm::vec2(64.0f, 64.0f));
    }
    
    sprite_renderer->endFrame();
    
    EXPECT_EQ(sprite_renderer->getSpriteCount(), 10u);
    EXPECT_EQ(sprite_renderer->getDrawCallCount(), 2u); // Two textures = exactly 2 batches
}

// RED PHASE: This test MUST fail - testing texture atlas integration
TEST_F(SpriteRendererTest, UsesTextureAtlases) {
    EXPECT_TRUE(sprite_renderer->initialize(*renderer, *texture_manager));
    
    // Create a texture atlas for testing
    d2::TextureAtlas atlas;
    
    // Add the atlas to the sprite renderer
    sprite_renderer->addAtlas(atlas);
    
    // Verify the atlas was added
    EXPECT_EQ(sprite_renderer->getAtlasCount(), 1u);
    
    // Test that we can draw sprites from the atlas
    sprite_renderer->beginFrame();
    sprite_renderer->drawSpriteFromAtlas("test_sprite", glm::vec2(0.0f, 0.0f), glm::vec2(64.0f, 64.0f));
    sprite_renderer->endFrame();
    
    // Atlas sprites should be batched efficiently
    EXPECT_EQ(sprite_renderer->getSpriteCount(), 1u);
    EXPECT_EQ(sprite_renderer->getDrawCallCount(), 1u);
}

// Phase 43: Performance Optimization - Texture Atlas Batching
TEST_F(SpriteRendererTest, BatchedRenderingWithTextureAtlas) {
    EXPECT_TRUE(sprite_renderer->initialize(*renderer, *texture_manager));
    
    // Create a mock texture atlas with multiple sprites
    d2::TextureAtlas atlas;
    sprite_renderer->addAtlas(atlas);
    
    // Render multiple sprites from the same atlas
    sprite_renderer->beginBatch();
    for (int i = 0; i < 10; i++) {
        std::string spriteName = "item" + std::to_string(i);
        sprite_renderer->drawSpriteFromAtlas(spriteName, glm::vec2(i * 64.0f, 0.0f), glm::vec2(64.0f, 64.0f));
    }
    sprite_renderer->endBatch();
    
    // All sprites from the same atlas should be batched into a single draw call
    EXPECT_EQ(sprite_renderer->getDrawCallCount(), 1);
    EXPECT_EQ(sprite_renderer->getSpriteCount(), 10);
}

// Phase 43: Performance Optimization - Vertex Buffer Pool Integration
TEST_F(SpriteRendererTest, UsesVertexBufferPool) {
    EXPECT_TRUE(sprite_renderer->initialize(*renderer, *texture_manager));
    
    // Create a vertex buffer pool
    auto pool = std::make_shared<VertexBufferPool>(5);
    sprite_renderer->setVertexBufferPool(pool);
    
    // Track initial pool state
    size_t initialPoolSize = pool->getPoolSize();
    size_t initialAvailableCount = pool->getAvailableCount();
    
    // Render many frames to test buffer reuse
    for (int frame = 0; frame < 10; frame++) {
        sprite_renderer->beginBatch();
        for (int i = 0; i < 20; i++) {
            sprite_renderer->drawSprite(1, glm::vec2(i * 32.0f, 0.0f), glm::vec2(32.0f, 32.0f));
        }
        sprite_renderer->endBatch();
    }
    
    // Pool should not have grown significantly (efficient reuse)
    EXPECT_LE(pool->getPoolSize() - initialPoolSize, 2);
    EXPECT_GT(pool->getAvailableCount(), 0); // Should have buffers available after rendering
}

} // namespace d2::rendering