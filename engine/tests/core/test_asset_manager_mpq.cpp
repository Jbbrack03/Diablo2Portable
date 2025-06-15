#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/mpq_loader.h"
#include <filesystem>
#include <fstream>

using namespace d2portable::core;
using namespace d2portable::utils;

class AssetManagerMPQTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for tests
        test_dir = std::filesystem::temp_directory_path() / "asset_manager_mpq_test";
        std::filesystem::create_directories(test_dir);
        
        // Create a test MPQ file with a DC6 sprite
        createTestMPQ();
    }
    
    void TearDown() override {
        // Clean up test directory
        std::filesystem::remove_all(test_dir);
    }
    
    void createTestMPQ() {
        // This creates a minimal MPQ with a test DC6 file
        test_mpq_path = test_dir / "test_data.mpq";
        
        // For now, we'll create a dummy MPQ file
        // In a real implementation, we'd use MPQ creation tools
        std::ofstream mpq_file(test_mpq_path, std::ios::binary);
        
        // Write MPQ header
        const char* signature = "MPQ\x1A";
        mpq_file.write(signature, 4);
        
        // Write minimal header (simplified for testing)
        uint32_t header_size = 44;
        uint32_t archive_size = 1024;
        uint16_t format_version = 0;
        uint16_t block_size = 9; // 512 bytes
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 128;
        uint32_t hash_table_entries = 16;
        uint32_t block_table_entries = 16;
        
        mpq_file.write(reinterpret_cast<const char*>(&header_size), 4);
        mpq_file.write(reinterpret_cast<const char*>(&archive_size), 4);
        mpq_file.write(reinterpret_cast<const char*>(&format_version), 2);
        mpq_file.write(reinterpret_cast<const char*>(&block_size), 2);
        mpq_file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
        mpq_file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
        mpq_file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
        mpq_file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
        
        // Close file for now (incomplete MPQ, but enough for initial test)
        mpq_file.close();
    }
    
    std::filesystem::path test_dir;
    std::filesystem::path test_mpq_path;
    AssetManager asset_manager;
};

// Test 1: Initialize AssetManager with MPQ file
TEST_F(AssetManagerMPQTest, InitializeWithMPQ) {
    // Initialize with MPQ file instead of directory
    EXPECT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    EXPECT_TRUE(asset_manager.isInitialized());
}

// Test 2: Check if file exists in MPQ (with empty test MPQ)
TEST_F(AssetManagerMPQTest, HasFileInMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Since our test MPQ is empty, we expect false
    EXPECT_FALSE(asset_manager.hasFile("data\\global\\ui\\cursor\\cursor.dc6"));
}

// Test 3: Load sprite from MPQ (negative test with empty MPQ)
TEST_F(AssetManagerMPQTest, LoadSpriteFromMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Load a sprite from the MPQ (should fail with empty MPQ)
    auto sprite = asset_manager.loadSprite("data\\global\\ui\\cursor\\cursor.dc6");
    EXPECT_EQ(sprite, nullptr);
}

// Test 4: Load raw file data from MPQ (negative test with empty MPQ)
TEST_F(AssetManagerMPQTest, LoadFileDataFromMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Load raw data from MPQ (should return empty with empty MPQ)
    auto data = asset_manager.loadFileData("data\\global\\excel\\armor.txt");
    EXPECT_TRUE(data.empty());
}

// Test 5: Initialize with directory containing MPQs
TEST_F(AssetManagerMPQTest, InitializeWithMPQDirectory) {
    // Create multiple MPQ files
    auto mpq2_path = test_dir / "d2exp.mpq";
    std::ofstream mpq2(mpq2_path, std::ios::binary);
    mpq2.write("MPQ\x1A", 4);
    mpq2.close();
    
    // Initialize with directory containing MPQs
    EXPECT_TRUE(asset_manager.initializeWithMPQs(test_dir.string()));
    
    // With empty MPQs, should not find any files
    EXPECT_FALSE(asset_manager.hasFile("data\\global\\ui\\cursor\\cursor.dc6"));
}

// Test 6: Fallback to filesystem when file not in MPQ
TEST_F(AssetManagerMPQTest, FallbackToFilesystem) {
    // Create a local file not in MPQ
    auto local_file = test_dir / "local_file.txt";
    std::ofstream file(local_file);
    file << "Local content";
    file.close();
    
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string(), test_dir.string()));
    
    // Should find local file that's not in MPQ
    EXPECT_TRUE(asset_manager.hasFile("local_file.txt"));
    
    auto data = asset_manager.loadFileData("local_file.txt");
    EXPECT_FALSE(data.empty());
    std::string content(data.begin(), data.end());
    EXPECT_EQ(content, "Local content");
}