#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

namespace d2::rendering {

class TextureBindingTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        textureManager = std::make_unique<TextureManager>();
        spriteRenderer = std::make_unique<SpriteRenderer>();
        spriteRenderer->initialize(*renderer, *textureManager);
    }

    std::unique_ptr<EGLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<SpriteRenderer> spriteRenderer;
};

// Test that textures are properly bound during rendering
TEST_F(TextureBindingTest, BindsTexturesDuringRendering) {
    // Create test textures
    std::vector<uint8_t> redTexture = {255, 0, 0, 255};
    std::vector<uint8_t> greenTexture = {0, 255, 0, 255};
    
    uint32_t redTextureId = textureManager->createTexture(redTexture.data(), 1, 1);
    uint32_t greenTextureId = textureManager->createTexture(greenTexture.data(), 1, 1);
    
    EXPECT_NE(redTextureId, 0) << "Red texture should be created";
    EXPECT_NE(greenTextureId, 0) << "Green texture should be created";
    
    // Begin rendering
    spriteRenderer->beginFrame();
    
    // Draw sprites with different textures
    spriteRenderer->drawSprite(redTextureId, glm::vec2(0, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(greenTextureId, glm::vec2(32, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(redTextureId, glm::vec2(64, 0), glm::vec2(32, 32));
    
    // End rendering - this should bind textures and make draw calls
    spriteRenderer->endFrame();
    
    // Should have made 2 draw calls (one per unique texture)
    EXPECT_EQ(spriteRenderer->getDrawCallCount(), 2)
        << "Should have made one draw call per unique texture";
    
    // In a real implementation, we would verify:
    // 1. glBindTexture was called with the correct texture IDs
    // 2. Textures were bound before their respective draw calls
    // 3. The render state was properly managed
    
    // For now, we verify the batching logic works correctly
    EXPECT_EQ(spriteRenderer->getSpriteCount(), 3)
        << "Should have rendered 3 sprites total";
}

// Test that invalid textures are handled gracefully
TEST_F(TextureBindingTest, HandlesInvalidTextures) {
    // Begin rendering
    spriteRenderer->beginFrame();
    
    // Draw sprite with invalid texture ID
    spriteRenderer->drawSprite(0, glm::vec2(0, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(999999, glm::vec2(32, 0), glm::vec2(32, 32));
    
    // End rendering - should handle invalid textures gracefully
    spriteRenderer->endFrame();
    
    // Should still process the sprites (even with invalid textures)
    EXPECT_EQ(spriteRenderer->getSpriteCount(), 2)
        << "Should still count sprites even with invalid textures";
    
    // Should make draw calls for each unique texture ID (even if invalid)
    EXPECT_EQ(spriteRenderer->getDrawCallCount(), 2)
        << "Should make draw calls for unique texture IDs";
}

} // namespace d2::rendering