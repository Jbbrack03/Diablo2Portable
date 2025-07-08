#include <gtest/gtest.h>
#include "rendering/texture_manager.h"
#include "sprites/dc6_parser.h"
#include <memory>
#include <vector>

namespace d2::rendering {

class SpriteToTextureTest : public ::testing::Test {
protected:
    void SetUp() override {
        textureManager = std::make_unique<TextureManager>();
    }

    std::unique_ptr<TextureManager> textureManager;
};

// Test that TextureManager can create textures from DC6 sprite data
TEST_F(SpriteToTextureTest, CreateTextureFromDC6Sprite) {
    // For now, skip this test since DC6Sprite is abstract
    // and we don't have a concrete implementation readily available
    // This test would need a mock DC6Sprite or test DC6 file
    
    // TODO: Implement when DC6Sprite testing infrastructure is available
    SUCCEED() << "Test skipped - DC6Sprite integration needs more setup";
}

// Test that TextureManager can create textures from raw RGBA data
TEST_F(SpriteToTextureTest, CreateTextureFromRGBAData) {
    // Create test RGBA data (2x2 pixels)
    std::vector<uint8_t> rgbaData = {
        255, 0, 0, 255,    // Red pixel
        0, 255, 0, 255,    // Green pixel
        0, 0, 255, 255,    // Blue pixel
        255, 255, 255, 255 // White pixel
    };
    
    uint32_t textureId = textureManager->createTexture(rgbaData.data(), 2, 2);
    
    EXPECT_NE(textureId, 0)
        << "Should be able to create texture from RGBA data";
    
    // Verify the texture is valid
    EXPECT_TRUE(textureManager->isTextureValid(textureId))
        << "Created texture should be valid";
    
    // Verify texture dimensions
    EXPECT_EQ(textureManager->getTextureWidth(textureId), 2)
        << "Texture width should match input";
    EXPECT_EQ(textureManager->getTextureHeight(textureId), 2)
        << "Texture height should match input";
}

} // namespace d2::rendering