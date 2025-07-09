#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include "sprites/dc6_parser.h"
#include "mock_dc6_sprite.h"

namespace d2::rendering {

class TextureManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple test sprite with proper mock
        test_sprite = std::make_shared<test::MockDC6Sprite>(1, 1, 2, 2);
        
        // Mock a simple 2x2 RGBA image
        test_sprite_data = {
            255, 0, 0, 255,    // Red pixel
            0, 255, 0, 255,    // Green pixel  
            0, 0, 255, 255,    // Blue pixel
            255, 255, 255, 255 // White pixel
        };
    }

    void TearDown() override {
        test_sprite.reset();
    }

    std::shared_ptr<sprites::DC6Sprite> test_sprite;
    std::vector<uint8_t> test_sprite_data;
};

// RED PHASE: This test MUST fail - testing DC6 sprite to OpenGL texture conversion
TEST_F(TextureManagerTest, UploadSpriteToTexture) {
    TextureManager manager;
    
    uint32_t texture_id = manager.uploadSprite(test_sprite, 0, 0); // direction 0, frame 0
    
    EXPECT_NE(texture_id, 0u);
    EXPECT_TRUE(manager.isTextureValid(texture_id));
}

// RED PHASE: This test MUST fail - testing raw RGBA data to OpenGL texture upload
TEST_F(TextureManagerTest, UploadRGBADataTexture) {
    TextureManager manager;
    
    // Create texture from raw RGBA data (2x2 pixels)
    uint32_t texture_id = manager.createTexture(test_sprite_data.data(), 2, 2);
    
    EXPECT_NE(texture_id, 0u) << "Texture should be created successfully from RGBA data";
    EXPECT_TRUE(manager.isTextureValid(texture_id)) << "Created texture should be valid";
    
    // Verify texture properties
    EXPECT_EQ(manager.getTextureWidth(texture_id), 2u) << "Texture width should match input";
    EXPECT_EQ(manager.getTextureHeight(texture_id), 2u) << "Texture height should match input";
}

} // namespace d2::rendering