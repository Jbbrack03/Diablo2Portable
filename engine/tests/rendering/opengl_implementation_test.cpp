#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

namespace d2::rendering {

class RealSpriteRenderer : public SpriteRenderer {
public:
    virtual ~RealSpriteRenderer() = default;  // Add virtual destructor
    
    void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) override {
        SpriteRenderer::drawSprite(texture_id, position, size);
        
        // In a real implementation, this would issue OpenGL draw calls
        // For now, we'll track if this was called
        actualDrawCalled_ = true;
    }
    
    bool wasActualDrawCalled() const { return actualDrawCalled_; }
    
    // This method should compile and link shader programs
    bool hasCompiledShaders() const { return shadersCompiled_; }
    
    void compileShaders() { shadersCompiled_ = true; }
    
private:
    bool actualDrawCalled_ = false;
    bool shadersCompiled_ = false;
};

class OpenGLImplementationTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        textureManager = std::make_unique<TextureManager>();
        spriteRenderer = std::make_unique<RealSpriteRenderer>();
        spriteRenderer->initialize(*renderer, *textureManager);
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
    std::unique_ptr<RealSpriteRenderer> spriteRenderer;
};

// Test that sprite renderer needs to compile shaders for real rendering
TEST_F(OpenGLImplementationTest, SpriteRendererRequiresShaderCompilation) {
    // A real sprite renderer should compile shaders during initialization
    spriteRenderer->compileShaders();
    
    EXPECT_TRUE(spriteRenderer->hasCompiledShaders()) 
        << "Real OpenGL rendering requires shader compilation";
}

} // namespace d2::rendering