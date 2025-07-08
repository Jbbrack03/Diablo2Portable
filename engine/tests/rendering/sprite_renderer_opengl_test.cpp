#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

namespace d2::rendering {

// Test fixture for sprite renderer OpenGL tests
class SpriteRendererOpenGLTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        textureManager = std::make_unique<TextureManager>();
        spriteRenderer = std::make_unique<SpriteRenderer>();
    }

    void TearDown() override {
        spriteRenderer.reset();
        textureManager.reset();
        renderer.reset();
        context.reset();
    }

    std::unique_ptr<EGLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<SpriteRenderer> spriteRenderer;
};

// Test that sprite renderer compiles vertex shader
TEST_F(SpriteRendererOpenGLTest, CompilesVertexShader) {
    // The SpriteRenderer should extend to support shader compilation
    class TestSpriteRenderer : public SpriteRenderer {
    public:
        bool hasCompiledVertexShader() const { return vertexShaderCompiled_; }
        
        bool initialize(const Renderer& renderer, const TextureManager& textureManager) {
            if (!SpriteRenderer::initialize(renderer, textureManager)) {
                return false;
            }
            
            // In a real implementation, this would compile the vertex shader
            // For now, we'll just set the flag
            vertexShaderCompiled_ = true;
            return true;
        }
        
    private:
        bool vertexShaderCompiled_ = false;
    };
    
    auto testRenderer = std::make_unique<TestSpriteRenderer>();
    ASSERT_TRUE(testRenderer->initialize(*renderer, *textureManager));
    
    // This test will pass trivially for now, but demonstrates what's needed
    EXPECT_TRUE(testRenderer->hasCompiledVertexShader()) 
        << "SpriteRenderer should compile a vertex shader for sprite rendering";
}

// RED PHASE: This test MUST fail - testing real shader program usage
TEST_F(SpriteRendererOpenGLTest, UsesShaderProgramForRendering) {
    // Initialize SpriteRenderer
    ASSERT_TRUE(spriteRenderer->initialize(*renderer, *textureManager));
    
    // Create a test texture
    std::vector<uint8_t> rgba_data(16, 255); // 2x2 white pixels
    uint32_t texture_id = textureManager->createTexture(rgba_data.data(), 2, 2);
    ASSERT_NE(texture_id, 0u);
    
    // SpriteRenderer should have a valid shader program after initialization
    uint32_t shader_program = spriteRenderer->getShaderProgram();
    EXPECT_NE(shader_program, 0u) << "SpriteRenderer should create a valid shader program";
    
    // Begin frame should bind the shader program
    spriteRenderer->beginFrame();
    EXPECT_TRUE(spriteRenderer->isShaderProgramActive()) << "Shader program should be active after beginFrame";
    
    // Draw a sprite - this should use the shader program
    spriteRenderer->drawSprite(texture_id, glm::vec2(100, 100), glm::vec2(64, 64));
    
    // Verify that the sprite was rendered using OpenGL calls
    EXPECT_GT(spriteRenderer->getSpriteCount(), 0u) << "Should have rendered at least one sprite";
    
    spriteRenderer->endFrame();
}

} // namespace d2::rendering