#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include "rendering/renderer.h"
#include "rendering/render_context.h"
#include "rendering/render_backend.h"
#include <vector>

namespace d2::rendering {

class TextureClampingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_NE(RenderContext::getBackend(), nullptr);
    }
};

TEST_F(TextureClampingTest, EnablesClampToEdgeForTextures) {
    Renderer renderer;
    TextureManager texture_manager;

    ASSERT_TRUE(texture_manager.initialize(renderer));

    std::vector<uint8_t> rgba_data = {255, 0, 0, 255};
    uint32_t texture_id = texture_manager.createTexture(rgba_data.data(), 1, 1);

    texture_manager.setTextureWrapMode(texture_id, TextureWrapMode::CLAMP_TO_EDGE);

    EXPECT_NE(texture_id, 0);
    EXPECT_TRUE(texture_manager.isTextureValid(texture_id));
}

TEST_F(TextureClampingTest, SupportsRepeatWrapMode) {
    Renderer renderer;
    TextureManager texture_manager;

    ASSERT_TRUE(texture_manager.initialize(renderer));

    std::vector<uint8_t> rgba_data = {0, 255, 0, 255};
    uint32_t texture_id = texture_manager.createTexture(rgba_data.data(), 1, 1);

    texture_manager.setTextureWrapMode(texture_id, TextureWrapMode::REPEAT);

    EXPECT_NE(texture_id, 0);
    EXPECT_TRUE(texture_manager.isTextureValid(texture_id));
}

} // namespace d2::rendering
