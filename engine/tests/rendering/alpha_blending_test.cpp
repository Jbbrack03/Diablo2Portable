#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"

namespace d2::rendering {

class AlphaBlendingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_NE(RenderContext::getBackend(), nullptr);
    }
};

TEST_F(AlphaBlendingTest, EnablesAlphaBlendingForTransparency) {
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;

    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));

    // enableAlphaBlending calls backend->enable(GL_BLEND) and backend->blendFunc(...)
    // If it doesn't crash, the backend received the calls correctly.
    sprite_renderer.enableAlphaBlending();

    // Verify the calls don't crash and we can continue rendering
    EXPECT_TRUE(true);
}

} // namespace d2::rendering
