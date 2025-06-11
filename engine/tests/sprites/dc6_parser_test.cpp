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
    
    void createRLETestDC6File() {
        rle_dc6_path = test_dir / "rle_sprite.dc6";
        std::ofstream file(rle_dc6_path, std::ios::binary);
        
        // DC6 Header
        uint32_t version = 6;
        uint32_t flags = 0;
        uint32_t encoding = 0;
        uint32_t termination = 0xEEEEEEEE;
        uint32_t directions = 1;  // Single direction
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
        
        // Frame header for 8x8 sprite
        uint32_t flip = 0;
        uint32_t width = 8;
        uint32_t height = 8;
        int32_t offset_x = -4;
        int32_t offset_y = -4;
        uint32_t allocsize = 0;
        uint32_t next_block = 0;
        
        // RLE compressed data will be much smaller than 64 bytes
        // Format: Row 0: [0x84, 1] [0x84, 2] = 4 pixels of color 1, 4 pixels of color 2
        //         Row 1: [0x88, 3] = 8 pixels of color 3
        //         Rows 2-3: [0x90, 0] = 16 pixels of transparent
        //         Rows 4-7: repeat pattern
        uint32_t length = 12; // RLE compressed length (estimated)
        
        file.write(reinterpret_cast<const char*>(&flip), 4);
        file.write(reinterpret_cast<const char*>(&width), 4);
        file.write(reinterpret_cast<const char*>(&height), 4);
        file.write(reinterpret_cast<const char*>(&offset_x), 4);
        file.write(reinterpret_cast<const char*>(&offset_y), 4);
        file.write(reinterpret_cast<const char*>(&allocsize), 4);
        file.write(reinterpret_cast<const char*>(&next_block), 4);
        file.write(reinterpret_cast<const char*>(&length), 4);
        
        // RLE compressed pixel data
        // DC6 RLE Format: [count | 0x80, pixel_value] for runs
        // Row 0: 4 pixels of color 1, then 4 pixels of color 2
        uint8_t rle_data[] = {
            0x84, 1,    // 4 pixels of color 1 (count=4, value=1)
            0x84, 2,    // 4 pixels of color 2 (count=4, value=2)
            0x88, 3,    // 8 pixels of color 3 (count=8, value=3) - Row 1
            0x90, 0,    // 16 pixels of transparent (count=16, value=0) - Rows 2-3
            0x80, 0     // End marker (count=0)
        };
        
        file.write(reinterpret_cast<const char*>(rle_data), sizeof(rle_data));
        
        // Update length to actual size
        file.seekp(frame_pointer + 28); // Seek to length field
        uint32_t actual_length = sizeof(rle_data);
        file.write(reinterpret_cast<const char*>(&actual_length), 4);
        
        file.close();
    }

    std::filesystem::path test_dir;
    std::filesystem::path test_dc6_path;
    std::filesystem::path rle_dc6_path;
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

// Test: Parse DC6 file with RLE compressed data
TEST_F(DC6ParserTest, ParseRLECompressedDC6) {
    // Create a DC6 file with RLE compressed pixel data
    createRLETestDC6File();
    
    DC6Parser parser;
    auto sprite = parser.parseFile(rle_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
    
    // Get the frame
    auto frame = sprite->getFrame(0, 0);
    EXPECT_EQ(frame.width, 8);
    EXPECT_EQ(frame.height, 8);
    
    // Get image data - should be properly decompressed
    auto image_data = sprite->getFrameImage(0, 0);
    EXPECT_EQ(image_data.size(), 8 * 8 * 4); // 8x8 RGBA
    
    // Verify decompressed pattern: 
    // Row 0: 4 pixels of color 1, 4 pixels of color 2
    // Row 1: 8 pixels of color 3
    // Rows 2-3: 16 pixels of transparent (color 0)
    // Rows 4-7: pattern repeats
    
    // Check first row: 4x color 1 (value 1), then 4x color 2 (value 2)
    for (int x = 0; x < 4; x++) {
        size_t idx = x * 4;
        EXPECT_EQ(image_data[idx], 1);     // R = palette index 1
        EXPECT_EQ(image_data[idx + 1], 1); // G = palette index 1  
        EXPECT_EQ(image_data[idx + 2], 1); // B = palette index 1
        EXPECT_EQ(image_data[idx + 3], 255); // A = opaque
    }
    
    for (int x = 4; x < 8; x++) {
        size_t idx = x * 4;
        EXPECT_EQ(image_data[idx], 2);     // R = palette index 2
        EXPECT_EQ(image_data[idx + 1], 2); // G = palette index 2
        EXPECT_EQ(image_data[idx + 2], 2); // B = palette index 2  
        EXPECT_EQ(image_data[idx + 3], 255); // A = opaque
    }
    
    // Check second row: 8x color 3 (value 3)
    for (int x = 0; x < 8; x++) {
        size_t idx = (8 + x) * 4; // Row 1
        EXPECT_EQ(image_data[idx], 3);     // R = palette index 3
        EXPECT_EQ(image_data[idx + 1], 3); // G = palette index 3
        EXPECT_EQ(image_data[idx + 2], 3); // B = palette index 3
        EXPECT_EQ(image_data[idx + 3], 255); // A = opaque
    }
}

// Test: Convert frame to image with custom palette
TEST_F(DC6ParserTest, ConvertFrameToImageWithPalette) {
    DC6Parser parser;
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    
    // Create a test palette (256 colors, RGBA format)
    std::vector<uint32_t> test_palette(256);
    for (size_t i = 0; i < 256; i++) {
        // Create a simple test palette where each index maps to a specific color
        uint8_t r = static_cast<uint8_t>(i);
        uint8_t g = static_cast<uint8_t>(255 - i);
        uint8_t b = static_cast<uint8_t>(i / 2);
        uint8_t a = 255;
        test_palette[i] = (a << 24) | (b << 16) | (g << 8) | r;
    }
    
    auto image_data = sprite->getFrameImageWithPalette(0, 0, test_palette);
    
    // Should have RGBA data (32x32x4 bytes)
    EXPECT_EQ(image_data.size(), 32 * 32 * 4);
    
    // Check that colors are correctly mapped through palette
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 32; x++) {
            size_t idx = (y * 32 + x) * 4;
            uint8_t original_index = ((x + y) % 2) ? 255 : 0;
            
            // Expected color from palette
            uint32_t palette_color = test_palette[original_index];
            uint8_t expected_r = palette_color & 0xFF;
            uint8_t expected_g = (palette_color >> 8) & 0xFF;
            uint8_t expected_b = (palette_color >> 16) & 0xFF;
            uint8_t expected_a = (palette_color >> 24) & 0xFF;
            
            EXPECT_EQ(image_data[idx], expected_r);     // R
            EXPECT_EQ(image_data[idx + 1], expected_g); // G
            EXPECT_EQ(image_data[idx + 2], expected_b); // B
            EXPECT_EQ(image_data[idx + 3], expected_a); // A
        }
    }
}

// Test: Parse and convert DC6 with palette using default Diablo II colors
TEST_F(DC6ParserTest, ConvertWithDefaultDiabloPalette) {
    DC6Parser parser;
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    
    // Test getting the default Diablo II palette
    auto default_palette = parser.getDefaultPalette();
    EXPECT_EQ(default_palette.size(), 256);
    
    // Palette index 0 should be transparent (alpha = 0)
    uint32_t transparent_color = default_palette[0];
    uint8_t alpha = (transparent_color >> 24) & 0xFF;
    EXPECT_EQ(alpha, 0);
    
    // Convert frame using default palette
    auto image_data = sprite->getFrameImageWithPalette(0, 0, default_palette);
    EXPECT_EQ(image_data.size(), 32 * 32 * 4);
    
    // Check that transparent pixels (index 0) have alpha = 0
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 32; x++) {
            size_t idx = (y * 32 + x) * 4;
            uint8_t original_index = ((x + y) % 2) ? 255 : 0;
            
            if (original_index == 0) {
                EXPECT_EQ(image_data[idx + 3], 0); // Alpha should be 0 for transparent
            } else {
                EXPECT_EQ(image_data[idx + 3], 255); // Alpha should be 255 for opaque
            }
        }
    }
}

// Test: Handle invalid palette size
TEST_F(DC6ParserTest, HandleInvalidPaletteSize) {
    DC6Parser parser;
    auto sprite = parser.parseFile(test_dc6_path.string());
    
    ASSERT_TRUE(sprite != nullptr);
    
    // Create an invalid palette (too small)
    std::vector<uint32_t> small_palette(100); // Only 100 colors instead of 256
    
    // Should fall back to grayscale conversion
    auto image_data = sprite->getFrameImageWithPalette(0, 0, small_palette);
    EXPECT_EQ(image_data.size(), 32 * 32 * 4);
    
    // Should use grayscale fallback
    for (size_t y = 0; y < 32; y++) {
        for (size_t x = 0; x < 32; x++) {
            size_t idx = (y * 32 + x) * 4;
            uint8_t expected = ((x + y) % 2) ? 255 : 0;
            EXPECT_EQ(image_data[idx], expected);     // R
            EXPECT_EQ(image_data[idx + 1], expected); // G
            EXPECT_EQ(image_data[idx + 2], expected); // B
            EXPECT_EQ(image_data[idx + 3], 255);      // A
        }
    }
}