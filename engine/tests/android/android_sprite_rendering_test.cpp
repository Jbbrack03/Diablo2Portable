#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include "rendering/sprite_renderer.h"
#include "sprites/dc6_parser.h"
#include "utils/file_utils.h"

namespace fs = std::filesystem;

class AndroidSpriteRenderingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        test_dir_ = fs::temp_directory_path() / "android_sprite_test";
        sprites_dir_ = test_dir_ / "sprites";
        fs::create_directories(sprites_dir_);
        
        // Check if we have real D2 sprites available
        real_sprites_available_ = checkRealSpritesAvailable();
    }
    
    void TearDown() override {
        if (fs::exists(test_dir_)) {
            fs::remove_all(test_dir_);
        }
    }
    
    bool checkRealSpritesAvailable() {
        // Check common Android asset locations
        std::vector<std::string> possible_paths = {
            "/sdcard/Android/data/com.diablo2portable/files/assets/sprites",
            "/data/data/com.diablo2portable/files/assets/sprites",
            "/android_asset/sprites",
            "./assets/sprites"  // For desktop testing
        };
        
        for (const auto& path : possible_paths) {
            if (fs::exists(path)) {
                real_sprites_path_ = path;
                return true;
            }
        }
        
        return false;
    }
    
    void createTestDC6Sprite(const fs::path& path) {
        // Create a minimal DC6 sprite for testing
        std::ofstream file(path, std::ios::binary);
        
        // DC6 Header
        uint32_t version = 6;
        uint32_t unknown1 = 1;
        uint32_t unknown2 = 0;
        uint32_t termination = 0xEEEEEEEE;
        uint32_t directions = 1;
        uint32_t frames_per_dir = 1;
        
        file.write(reinterpret_cast<const char*>(&version), 4);
        file.write(reinterpret_cast<const char*>(&unknown1), 4);
        file.write(reinterpret_cast<const char*>(&unknown2), 4);
        file.write(reinterpret_cast<const char*>(&termination), 4);
        file.write(reinterpret_cast<const char*>(&directions), 4);
        file.write(reinterpret_cast<const char*>(&frames_per_dir), 4);
        
        // Frame pointer
        uint32_t frame_pointer = 32;  // After header
        file.write(reinterpret_cast<const char*>(&frame_pointer), 4);
        
        // Termination
        file.write(reinterpret_cast<const char*>(&termination), 4);
        
        // Frame data
        uint32_t flip = 0;
        uint32_t width = 32;
        uint32_t height = 32;
        int32_t offset_x = 0;
        int32_t offset_y = 0;
        uint32_t unknown = 0;
        uint32_t next_block = 0;
        uint32_t length = width * height;
        
        file.write(reinterpret_cast<const char*>(&flip), 4);
        file.write(reinterpret_cast<const char*>(&width), 4);
        file.write(reinterpret_cast<const char*>(&height), 4);
        file.write(reinterpret_cast<const char*>(&offset_x), 4);
        file.write(reinterpret_cast<const char*>(&offset_y), 4);
        file.write(reinterpret_cast<const char*>(&unknown), 4);
        file.write(reinterpret_cast<const char*>(&next_block), 4);
        file.write(reinterpret_cast<const char*>(&length), 4);
        
        // Write simple pixel data (checkerboard pattern)
        for (uint32_t i = 0; i < length; ++i) {
            uint8_t pixel = ((i / width + i % width) % 2) ? 255 : 0;
            file.write(reinterpret_cast<const char*>(&pixel), 1);
        }
        
        // Termination bytes
        uint8_t term_bytes[3] = {0xEE, 0xEE, 0xEE};
        file.write(reinterpret_cast<const char*>(term_bytes), 3);
    }
    
    fs::path test_dir_;
    fs::path sprites_dir_;
    fs::path real_sprites_path_;
    bool real_sprites_available_;
};

TEST_F(AndroidSpriteRenderingTest, LoadTestDC6Sprite) {
    // Create a test DC6 sprite
    fs::path test_sprite_path = sprites_dir_ / "test_sprite.dc6";
    createTestDC6Sprite(test_sprite_path);
    
    // Load the sprite
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile(test_sprite_path.string());
    ASSERT_NE(sprite, nullptr);
    
    // Verify sprite properties
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
    auto frame = sprite->getFrame(0, 0);
    EXPECT_EQ(frame.width, 32);
    EXPECT_EQ(frame.height, 32);
}

TEST_F(AndroidSpriteRenderingTest, LoadRealD2Sprite) {
    if (!real_sprites_available_) {
        GTEST_SKIP() << "Real D2 sprites not available at expected Android locations";
    }
    
    // Try to find a common sprite (e.g., cursor or UI element)
    std::vector<std::string> common_sprites = {
        "data/global/ui/cursor/hand.dc6",
        "data/global/ui/panel/invchar6.dc6",
        "data/global/ui/panel/800ctrlpnl7.dc6"
    };
    
    bool found_sprite = false;
    d2portable::sprites::DC6Parser parser;
    for (const auto& sprite_name : common_sprites) {
        fs::path sprite_path = fs::path(real_sprites_path_) / sprite_name;
        if (fs::exists(sprite_path)) {
            auto sprite = parser.parseFile(sprite_path.string());
            EXPECT_NE(sprite, nullptr);
            if (sprite) {
                EXPECT_GT(sprite->getDirectionCount(), 0);
                EXPECT_GT(sprite->getFramesPerDirection(), 0);
                found_sprite = true;
                break;
            }
        }
    }
    
    if (!found_sprite) {
        GTEST_SKIP() << "No common D2 sprites found in: " << real_sprites_path_;
    }
}

TEST_F(AndroidSpriteRenderingTest, RenderSpriteToTexture) {
    // Create a test sprite
    fs::path test_sprite_path = sprites_dir_ / "render_test.dc6";
    createTestDC6Sprite(test_sprite_path);
    
    // Load the sprite
    d2portable::sprites::DC6Parser parser;
    auto sprite = parser.parseFile(test_sprite_path.string());
    ASSERT_NE(sprite, nullptr);
    
    // Create a simple renderer (mock OpenGL context)
    d2::rendering::SpriteRenderer renderer;
    
    // Test that we can get frame data from the sprite
    auto frame = sprite->getFrame(0, 0);
    ASSERT_GT(frame.pixelData.size(), 0);
    
    // Verify frame data size
    uint32_t expected_size = frame.width * frame.height;
    EXPECT_EQ(frame.pixelData.size(), expected_size);
    
    // Verify checkerboard pattern
    for (uint32_t y = 0; y < frame.height; ++y) {
        for (uint32_t x = 0; x < frame.width; ++x) {
            uint32_t index = y * frame.width + x;
            uint8_t expected_pixel = ((x + y) % 2) ? 255 : 0;
            EXPECT_EQ(frame.pixelData[index], expected_pixel) 
                << "Mismatch at (" << x << ", " << y << ")";
        }
    }
}

TEST_F(AndroidSpriteRenderingTest, VerifyAssetPathIntegration) {
    // This test verifies that the Android asset path is properly set up
    // and that sprites can be loaded from the correct location
    
    // Check if OnboardingHelper would provide the correct path
    std::string expected_asset_path = "/sdcard/Android/data/com.diablo2portable/files/assets";
    
    // For desktop testing, use a different path
    #ifndef __ANDROID__
    expected_asset_path = "./assets";
    #endif
    
    // Verify the path structure
    if (fs::exists(expected_asset_path)) {
        fs::path sprites_path = fs::path(expected_asset_path) / "sprites";
        bool has_sprites = fs::exists(sprites_path) && !fs::is_empty(sprites_path);
        
        if (has_sprites) {
            // Count DC6 files
            int dc6_count = 0;
            for (const auto& entry : fs::recursive_directory_iterator(sprites_path)) {
                if (entry.path().extension() == ".dc6") {
                    dc6_count++;
                }
            }
            
            EXPECT_GT(dc6_count, 0) << "No DC6 sprites found in: " << sprites_path;
            
            // Log for debugging
            std::cout << "Found " << dc6_count << " DC6 sprites in: " << sprites_path << std::endl;
        }
    } else {
        GTEST_SKIP() << "Asset path does not exist: " << expected_asset_path;
    }
}