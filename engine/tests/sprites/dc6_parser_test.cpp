#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sprites/dc6_parser.h"
#include <fstream>
#include <filesystem>

using namespace d2portable::sprites;
using namespace testing;

class DC6ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = std::filesystem::temp_directory_path() / "d2portable_dc6_test";
        std::filesystem::create_directories(test_dir);
        
        // Create a test DC6 file
        test_dc6_path = test_dir / "test_sprite.dc6";
        createTestDC6File();
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }

    void createTestDC6File() {
        std::ofstream file(test_dc6_path, std::ios::binary);
        
        // DC6 Header
        uint32_t version = 6;
        uint32_t flags = 0;
        uint32_t encoding = 0;
        uint32_t termination = 0xEEEEEEEE;
        uint32_t directions = 1;  // Single direction for simple test
        uint32_t frames_per_dir = 1;  // Single frame
        
        file.write(reinterpret_cast<const char*>(&version), 4);
        file.write(reinterpret_cast<const char*>(&flags), 4);
        file.write(reinterpret_cast<const char*>(&encoding), 4);
        file.write(reinterpret_cast<const char*>(&termination), 4);
        file.write(reinterpret_cast<const char*>(&directions), 4);
        file.write(reinterpret_cast<const char*>(&frames_per_dir), 4);
        
        // Frame pointers (1 frame)
        uint32_t frame_pointer = 256;  // Frame data starts at offset 256
        file.write(reinterpret_cast<const char*>(&frame_pointer), 4);
        
        // Pad to frame data
        file.seekp(frame_pointer);
        
        // Frame header
        uint32_t flip = 0;
        uint32_t width = 32;
        uint32_t height = 32;
        int32_t offset_x = -16;
        int32_t offset_y = -16;
        uint32_t allocsize = 0;
        uint32_t next_block = 0;
        uint32_t length = width * height;  // Uncompressed
        
        file.write(reinterpret_cast<const char*>(&flip), 4);
        file.write(reinterpret_cast<const char*>(&width), 4);
        file.write(reinterpret_cast<const char*>(&height), 4);
        file.write(reinterpret_cast<const char*>(&offset_x), 4);
        file.write(reinterpret_cast<const char*>(&offset_y), 4);
        file.write(reinterpret_cast<const char*>(&allocsize), 4);
        file.write(reinterpret_cast<const char*>(&next_block), 4);
        file.write(reinterpret_cast<const char*>(&length), 4);
        
        // Simple pixel data (checkerboard pattern)
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                uint8_t pixel = ((x + y) % 2) ? 255 : 0;
                file.write(reinterpret_cast<const char*>(&pixel), 1);
            }
        }
        
        // Termination bytes
        file.write(reinterpret_cast<const char*>(&termination), 4);
    }

    std::filesystem::path test_dir;
    std::filesystem::path test_dc6_path;
};

// Test: Parse a valid DC6 file
TEST_F(DC6ParserTest, ParseValidDC6) {
    DC6Parser parser;
    
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    EXPECT_TRUE(sprite != nullptr);
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
}

// Test: Get frame dimensions
TEST_F(DC6ParserTest, GetFrameDimensions) {
    DC6Parser parser;
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    
    auto frame = sprite->getFrame(0, 0);  // Direction 0, Frame 0
    EXPECT_EQ(frame.width, 32);
    EXPECT_EQ(frame.height, 32);
    EXPECT_EQ(frame.offset_x, -16);
    EXPECT_EQ(frame.offset_y, -16);
}

// Test: Convert frame to image data
TEST_F(DC6ParserTest, ConvertFrameToImage) {
    DC6Parser parser;
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    
    auto image_data = sprite->getFrameImage(0, 0);
    
    // Should have RGBA data (32x32x4 bytes)
    EXPECT_EQ(image_data.size(), 32 * 32 * 4);
    
    // Check checkerboard pattern
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 32; x++) {
            size_t idx = (y * 32 + x) * 4;
            uint8_t expected = ((x + y) % 2) ? 255 : 0;
            EXPECT_EQ(image_data[idx], expected);     // R
            EXPECT_EQ(image_data[idx + 1], expected); // G
            EXPECT_EQ(image_data[idx + 2], expected); // B
            EXPECT_EQ(image_data[idx + 3], 255);      // A (always opaque for now)
        }
    }
}