#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include "sprites/dc6_sprite_wrapper.h"
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

// Mock DC6Sprite implementation for testing
class MockDC6Sprite : public d2::sprites::DC6Sprite {
public:
    MOCK_METHOD(uint32_t, getDirectionCount, (), (const, override));
    MOCK_METHOD(uint32_t, getFramesPerDirection, (), (const, override));
    MOCK_METHOD(d2::sprites::DC6Frame, getFrame, (uint32_t, uint32_t), (const, override));
    MOCK_METHOD(std::vector<uint8_t>, getFrameImage, (uint32_t, uint32_t), (const, override));
    MOCK_METHOD(std::vector<uint8_t>, getFrameImageWithPalette, 
                (uint32_t, uint32_t, const std::vector<uint32_t>&), (const, override));
};

// Test that TextureManager can create textures from DC6 sprite data
TEST_F(SpriteToTextureTest, CreateTextureFromDC6Sprite) {
    // Create a mock DC6 sprite
    auto mockSprite = std::make_unique<MockDC6Sprite>();
    
    // Set up expectations for the mock
    EXPECT_CALL(*mockSprite, getDirectionCount())
        .WillOnce(testing::Return(1));
    EXPECT_CALL(*mockSprite, getFramesPerDirection())
        .WillOnce(testing::Return(1));
    
    // Create test RGBA data for a 2x2 sprite
    std::vector<uint8_t> rgbaData = {
        255, 0, 0, 255,    // Red pixel
        0, 255, 0, 255,    // Green pixel
        0, 0, 255, 255,    // Blue pixel
        255, 255, 255, 255 // White pixel
    };
    
    EXPECT_CALL(*mockSprite, getFrameImage(0, 0))
        .WillOnce(testing::Return(rgbaData));
    
    // Create texture from DC6 sprite
    uint32_t textureId = textureManager->uploadSprite(
        std::shared_ptr<d2::sprites::DC6Sprite>(mockSprite.release()), 0, 0);
    
    EXPECT_NE(textureId, 0)
        << "Should be able to create texture from DC6 sprite";
    
    // Verify the texture is valid
    EXPECT_TRUE(textureManager->isTextureValid(textureId))
        << "Created texture should be valid";
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

// Test that TextureManager properly handles DC6 frame dimensions
TEST_F(SpriteToTextureTest, CreateTextureFromDC6SpriteWithProperDimensions) {
    // Create a mock DC6 sprite
    auto mockSprite = std::make_unique<MockDC6Sprite>();
    
    // Set up expectations for the mock
    EXPECT_CALL(*mockSprite, getDirectionCount())
        .WillOnce(testing::Return(8)); // 8 directions typical for Diablo II
    EXPECT_CALL(*mockSprite, getFramesPerDirection())
        .WillOnce(testing::Return(16)); // Multiple frames per direction
    
    // Create test DC6Frame data with specific dimensions
    d2::sprites::DC6Frame testFrame;
    testFrame.width = 32;
    testFrame.height = 48;
    testFrame.offset_x = -16;
    testFrame.offset_y = -40;
    
    // Create RGBA data matching the frame dimensions (32x48)
    std::vector<uint8_t> rgbaData(32 * 48 * 4, 255); // All white pixels
    
    EXPECT_CALL(*mockSprite, getFrame(3, 7)) // Direction 3, Frame 7
        .WillOnce(testing::Return(testFrame));
    EXPECT_CALL(*mockSprite, getFrameImage(3, 7))
        .WillOnce(testing::Return(rgbaData));
    
    // Create texture from specific frame
    uint32_t textureId = textureManager->uploadSprite(
        std::shared_ptr<d2::sprites::DC6Sprite>(mockSprite.release()), 3, 7);
    
    EXPECT_NE(textureId, 0)
        << "Should be able to create texture from specific DC6 frame";
    
    // Verify texture dimensions match the frame
    EXPECT_EQ(textureManager->getTextureWidth(textureId), 32)
        << "Texture width should match DC6 frame width";
    EXPECT_EQ(textureManager->getTextureHeight(textureId), 48)
        << "Texture height should match DC6 frame height";
}

} // namespace d2::rendering