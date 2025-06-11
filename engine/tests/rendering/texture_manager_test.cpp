#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rendering/texture_manager.h"
#include "sprites/dc6_parser.h"

namespace d2::rendering {

class TextureManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple test sprite (using nullptr for minimal test)
        test_sprite = nullptr;
        
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

} // namespace d2::rendering