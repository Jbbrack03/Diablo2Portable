#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/mock_mpq_builder.h"
#include <filesystem>
#include <vector>

using namespace d2portable::core;
using namespace d2portable::utils;

// Integration tests using mock MPQ files to avoid requiring real game files
class MockMPQIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory for mock MPQ files
        test_dir = std::filesystem::temp_directory_path() / "d2portable_mock_mpq_test";
        std::filesystem::create_directories(test_dir);
        
        mock_mpq_path = test_dir / "mock_d2data.mpq";
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }
    
    std::filesystem::path test_dir;
    std::filesystem::path mock_mpq_path;
    AssetManager asset_manager;
};

// Test 1: Create and load a mock MPQ file
TEST_F(MockMPQIntegrationTest, CreateAndLoadMockMPQ) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Add some test files to the mock MPQ
    std::vector<uint8_t> test_data = {'T', 'E', 'S', 'T'};
    builder.addFile("data\\test\\file1.txt", test_data);
    
    // Add a mock DC6 sprite (minimal valid DC6 header)
    std::vector<uint8_t> dc6_header(24, 0);
    // DC6 version (6)
    *reinterpret_cast<uint32_t*>(&dc6_header[0]) = 6;
    // Flags
    *reinterpret_cast<uint32_t*>(&dc6_header[4]) = 0;
    // Encoding
    *reinterpret_cast<uint32_t*>(&dc6_header[8]) = 0;
    // Termination
    *reinterpret_cast<uint32_t*>(&dc6_header[12]) = 0xEEEEEEEE;
    // Directions (1)
    *reinterpret_cast<uint32_t*>(&dc6_header[16]) = 1;
    // Frames per direction (1)
    *reinterpret_cast<uint32_t*>(&dc6_header[20]) = 1;
    
    builder.addFile("data\\global\\ui\\cursor\\test.dc6", dc6_header);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Now try to load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Verify we can access the files
    EXPECT_TRUE(asset_manager.hasFile("data\\test\\file1.txt"));
    EXPECT_TRUE(asset_manager.hasFile("data\\global\\ui\\cursor\\test.dc6"));
}

// Test 2: Extract and validate DC6 sprite from mock MPQ
TEST_F(MockMPQIntegrationTest, ExtractDC6FromMockMPQ) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Create a more complete DC6 file with direction and frame data
    std::vector<uint8_t> dc6_data;
    
    // DC6 Header (24 bytes)
    // Version (6)
    dc6_data.insert(dc6_data.end(), {0x06, 0x00, 0x00, 0x00});
    // Flags (1 = has direction data)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    // Encoding (0 = uncompressed)
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Termination bytes
    dc6_data.insert(dc6_data.end(), {0xEE, 0xEE, 0xEE, 0xEE});
    // Directions (1)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    // Frames per direction (1)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    
    // Frame pointer (follows header)
    // For 1 direction, 1 frame per direction, we have 1 frame pointer
    uint32_t frame_offset = 24 + 4; // Header + one frame pointer
    dc6_data.insert(dc6_data.end(), {
        static_cast<uint8_t>(frame_offset & 0xFF),
        static_cast<uint8_t>((frame_offset >> 8) & 0xFF),
        static_cast<uint8_t>((frame_offset >> 16) & 0xFF),
        static_cast<uint8_t>((frame_offset >> 24) & 0xFF)
    });
    
    
    // Frame header (32 bytes)
    // Flip
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Width
    dc6_data.insert(dc6_data.end(), {0x10, 0x00, 0x00, 0x00}); // 16 pixels
    // Height
    dc6_data.insert(dc6_data.end(), {0x10, 0x00, 0x00, 0x00}); // 16 pixels
    // OffsetX
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // OffsetY
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Unknown
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // NextBlock
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Length
    dc6_data.insert(dc6_data.end(), {0x00, 0x01, 0x00, 0x00}); // 256 bytes of pixel data
    
    // Add some pixel data (16x16 = 256 bytes)
    for (int i = 0; i < 256; i++) {
        dc6_data.push_back(static_cast<uint8_t>(i & 0xFF));
    }
    
    builder.addFile("data\\global\\ui\\cursor\\hand.dc6", dc6_data);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Now try to load the DC6 sprite
    auto sprite = asset_manager.loadSprite("data\\global\\ui\\cursor\\hand.dc6");
    ASSERT_NE(sprite, nullptr);
    
    // Validate the sprite properties
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
    
    // Get frame data
    auto frame = sprite->getFrame(0, 0);
    EXPECT_EQ(frame.width, 16);
    EXPECT_EQ(frame.height, 16);
}