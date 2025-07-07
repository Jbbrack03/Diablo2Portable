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