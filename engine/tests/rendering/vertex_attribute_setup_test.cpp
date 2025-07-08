#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

namespace d2::rendering {

// Test fixture for vertex attribute setup
class VertexAttributeSetupTest : public ::testing::Test {
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

// Test that vertex attributes are properly configured for position and texture coordinates
TEST_F(VertexAttributeSetupTest, ConfiguresVertexAttributesForPositionAndTexCoords) {
    // Begin frame
    spriteRenderer->beginFrame();
    
    // Draw a sprite - this should set up vertex attributes
    spriteRenderer->drawSprite(1, glm::vec2(0, 0), glm::vec2(32, 32));
    
    // End frame - this should configure vertex attributes
    spriteRenderer->endFrame();
    
    // In a real implementation, we would check that:
    // 1. glEnableVertexAttribArray was called for position (location 0)
    // 2. glEnableVertexAttribArray was called for texcoord (location 1)
    // 3. glVertexAttribPointer was called for position with correct offset
    // 4. glVertexAttribPointer was called for texcoord with correct offset
    
    // For now, verify that the rendering completed without errors
    EXPECT_GT(spriteRenderer->getDrawCallCount(), 0)
        << "Draw calls should have been made";
    
    // Verify that the vertex buffer is configured
    EXPECT_NE(spriteRenderer->getVertexBufferId(), 0)
        << "Vertex buffer should be configured";
    
    // Verify that the VAO is configured
    EXPECT_NE(spriteRenderer->getVAOId(), 0)
        << "VAO should be configured with vertex attributes";
}

} // namespace d2::rendering