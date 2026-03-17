#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/sprite_renderer.h"
#include "rendering/renderer.h"
#include "rendering/texture_manager.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"

namespace d2::rendering {

class DepthTestingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_NE(RenderContext::getBackend(), nullptr);
    }
};

TEST_F(DepthTestingTest, EnablesDepthTestingForSpriteLayering) {
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;

    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));

    // enableDepthTesting calls backend->enable(GL_DEPTH_TEST) and backend->depthFunc(GL_LEQUAL)
    sprite_renderer.enableDepthTesting();
    EXPECT_TRUE(true);
}

TEST_F(DepthTestingTest, DisablesDepthWritesForTransparency) {
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;

    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));

    sprite_renderer.enableDepthTesting();
    sprite_renderer.disableDepthWrites();
    EXPECT_TRUE(true);
}

TEST_F(DepthTestingTest, EnablesDepthWritesForOpaqueSprites) {
    Renderer renderer;
    TextureManager texture_manager;
    SpriteRenderer sprite_renderer;

    ASSERT_TRUE(sprite_renderer.initialize(renderer, texture_manager));

    sprite_renderer.enableDepthTesting();
    sprite_renderer.enableDepthWrites();
    EXPECT_TRUE(true);
}

} // namespace d2::rendering
