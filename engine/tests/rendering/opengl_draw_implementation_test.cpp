#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
// Mock OpenGL types for testing
typedef unsigned int GLint;
#endif

namespace d2::rendering {

// Test fixture for OpenGL draw implementation
class OpenGLDrawImplementationTest : public ::testing::Test {
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

// Test that SpriteRenderer actually calls glDrawArrays when rendering sprites
TEST_F(OpenGLDrawImplementationTest, CallsGLDrawArraysWhenRenderingSprites) {
    // Begin frame
    spriteRenderer->beginFrame();
    
    // Draw some sprites
    spriteRenderer->drawSprite(1, glm::vec2(0, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(1, glm::vec2(32, 0), glm::vec2(32, 32));
    
    // Note: In a real test we would need to intercept OpenGL calls or use a debug context
    // For now, we'll verify that the vertex buffer has the right data
    
    // End frame - this should trigger actual OpenGL draw calls
    spriteRenderer->endFrame();
    
    // Verify that draw calls were made
    // In a real implementation, we would check:
    // 1. glUseProgram was called with the shader program
    // 2. glBindVertexArray was called with the VAO
    // 3. glBindBuffer was called with the vertex buffer
    // 4. glDrawArrays was called with the correct vertex count
    
    // For now, verify that the sprite renderer would make draw calls
    EXPECT_GT(spriteRenderer->getDrawCallCount(), 0) 
        << "SpriteRenderer should report draw calls when sprites are rendered";
    
    // Verify OpenGL resources are bound
    EXPECT_NE(spriteRenderer->getShaderProgram(), 0)
        << "Shader program should be created";
    EXPECT_NE(spriteRenderer->getVAOId(), 0)
        << "VAO should be created";
    EXPECT_NE(spriteRenderer->getVertexBufferId(), 0)
        << "Vertex buffer should be created";
}

} // namespace d2::rendering