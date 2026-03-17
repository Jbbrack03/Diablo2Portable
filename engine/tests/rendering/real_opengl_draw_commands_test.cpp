#include <gtest/gtest.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/egl_context.h"
#include "rendering/render_context.h"
#include "rendering/mock_render_backend.h"
#include <glm/glm.hpp>

namespace d2::rendering {

class RealOpenGLDrawCommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto* backend = dynamic_cast<MockRenderBackend*>(RenderContext::getBackend());
        if (backend) backend->resetDrawCommandTracking();

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

// Test that SpriteRenderer actually calls real OpenGL draw commands
TEST_F(RealOpenGLDrawCommandsTest, ActuallyCallsRealOpenGLDrawCommands) {
    auto* backend = dynamic_cast<MockRenderBackend*>(RenderContext::getBackend());
    ASSERT_NE(backend, nullptr);

    // Create a texture for testing
    std::vector<uint8_t> texture_data(32 * 32 * 4, 255); // 32x32 RGBA white texture
    uint32_t texture_id = textureManager->createTexture(texture_data.data(), 32, 32);
    ASSERT_NE(texture_id, 0u) << "Failed to create test texture";

    // Begin frame
    spriteRenderer->beginFrame();

    // Draw sprites that should generate vertices
    spriteRenderer->drawSprite(texture_id, glm::vec2(0, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(texture_id, glm::vec2(32, 0), glm::vec2(32, 32));
    spriteRenderer->drawSprite(texture_id, glm::vec2(64, 0), glm::vec2(32, 32));

    // End frame - this should make actual OpenGL draw command calls
    spriteRenderer->endFrame();

    // Verify that real OpenGL draw commands were called
    EXPECT_TRUE(backend->wasDrawArraysCalled())
        << "SpriteRenderer should call actual glDrawArrays for rendering sprites";

    // Verify correct number of draw calls (should be 1 batch for same texture)
    EXPECT_EQ(backend->getDrawArraysCallCount(), 1u)
        << "Should make exactly one draw call for sprites with same texture";

    // Verify the draw call parameters are correct
    const auto& draw_calls = backend->getDrawArraysCalls();
    if (!draw_calls.empty()) {
        const auto& call = draw_calls[0];
        EXPECT_EQ(call.mode, 0x0004u)  // GL_TRIANGLES
            << "Should use GL_TRIANGLES mode for sprite rendering";
        EXPECT_EQ(call.first, 0)
            << "Should start drawing from vertex 0";
        EXPECT_EQ(call.count, 18)  // 3 sprites * 6 vertices per sprite
            << "Should draw correct number of vertices (3 sprites * 6 vertices per sprite)";
    }
}

} // namespace d2::rendering
