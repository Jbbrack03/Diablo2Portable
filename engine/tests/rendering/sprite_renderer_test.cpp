#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/egl_context.h"
#include "rendering/texture_manager.h"
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

// RED PHASE: This test MUST fail - testing single sprite rendering
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

} // namespace d2::rendering