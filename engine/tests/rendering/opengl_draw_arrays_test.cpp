#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include <glm/glm.hpp>

namespace d2::rendering {

// Extended SpriteRenderer that tracks if actual draw calls are made
class DrawCallTrackingSpriteRenderer : public SpriteRenderer {
public:
    void endFrame() override {
        // Call parent implementation
        SpriteRenderer::endFrame();
        
        // In the current implementation, this should call glDrawArrays
        // but it doesn't - this test will fail
        actualDrawCallsMade_ = checkIfDrawCallsWereMade();
    }
    
    bool wereActualDrawCallsMade() const {
        return actualDrawCallsMade_;
    }
    
private:
    bool actualDrawCallsMade_ = false;
    
    bool checkIfDrawCallsWereMade() {
        // Check if the sprite renderer actually batches vertices and calls glDrawArrays
        // If we have sprites and the draw call count matches texture count, we made real draw calls
        return getSpriteCount() > 0 && getDrawCallCount() > 0;
    }
};

class OpenGLDrawArraysTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<EGLContext>();
        context->initialize();
        
        renderer = std::make_unique<Renderer>();
        renderer->initialize(*context);
        
        textureManager = std::make_unique<TextureManager>();
        spriteRenderer = std::make_unique<DrawCallTrackingSpriteRenderer>();
        spriteRenderer->initialize(*renderer, *textureManager);
    }

    std::unique_ptr<EGLContext> context;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<DrawCallTrackingSpriteRenderer> spriteRenderer;
};

// Test that SpriteRenderer actually batches vertices and calls glDrawArrays
TEST_F(OpenGLDrawArraysTest, ActuallyCallsGLDrawArrays) {
    // Begin frame
    spriteRenderer->beginFrame();
    
    // Draw some sprites - these should be batched into vertex data
    spriteRenderer->drawSprite(1, glm::vec2(0, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(1, glm::vec2(32, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(2, glm::vec2(64, 0), glm::vec2(32, 32));
    
    // End frame - this should batch vertices and call glDrawArrays
    spriteRenderer->endFrame();
    
    // Verify that actual OpenGL draw calls were made
    EXPECT_TRUE(spriteRenderer->wereActualDrawCallsMade())
        << "SpriteRenderer should make actual OpenGL draw calls (glDrawArrays) when rendering sprites";
}

} // namespace d2::rendering