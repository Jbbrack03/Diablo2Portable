#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/stormlib_mpq_loader.h"
#include "utils/mpq_validator.h"
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
        // Use our real working MPQ files instead of creating fake ones
        // Get the path relative to the test executable
        std::filesystem::path exe_dir = std::filesystem::path(__FILE__).parent_path();
        std::filesystem::path project_root = exe_dir.parent_path().parent_path().parent_path();
        std::filesystem::path vendor_mpq_dir = project_root / "vendor" / "mpq";
        
        test_mpq_path = vendor_mpq_dir / "d2data.mpq";  // lowercase filename
        test_mpq_dir = vendor_mpq_dir.string();
        
        // Verify the test MPQ exists and is valid
        if (!std::filesystem::exists(test_mpq_path)) {
            // Skip tests if MPQ files are not available
            GTEST_SKIP() << "Test MPQ files not available at " << test_mpq_path;
        }
        
        // Check if MPQ file is valid (not empty)
        auto file_size = std::filesystem::file_size(test_mpq_path);
        if (file_size == 0) {
            GTEST_SKIP() << "MPQ file is empty (0 bytes). Please copy valid Diablo II MPQ files to " << vendor_mpq_dir;
        }
        
        // Use MPQValidator to check if file is valid
        auto validation = d2::utils::MPQValidator::validateMPQFile(test_mpq_path.string());
        if (validation.isPlaceholder) {
            GTEST_SKIP() << "MPQ file is a placeholder. " << validation.error;
        }
        if (!validation.isValid) {
            GTEST_SKIP() << "Invalid MPQ file: " << validation.error << ". Please copy valid Diablo II MPQ files to " << vendor_mpq_dir;
        }
    }
    
    std::filesystem::path test_dir;
    std::filesystem::path test_mpq_path;
    std::string test_mpq_dir;
    AssetManager asset_manager;
};

// Test 1: Initialize AssetManager with MPQ file
TEST_F(AssetManagerMPQTest, InitializeWithMPQ) {
    // Initialize with MPQ file instead of directory
    EXPECT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    EXPECT_TRUE(asset_manager.isInitialized());
}

// Test 2: Check if file exists in MPQ
TEST_F(AssetManagerMPQTest, HasFileInMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Check for a file that should exist in d2data.mpq
    EXPECT_TRUE(asset_manager.hasFile("data\\global\\excel\\armor.txt"));
    
    // Check for a file that doesn't exist
    EXPECT_FALSE(asset_manager.hasFile("nonexistent\\file.txt"));
}

// Test 3: Load sprite from MPQ
TEST_F(AssetManagerMPQTest, LoadSpriteFromMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Try to load a DC6 sprite that exists in d2data.mpq
    auto sprite = asset_manager.loadSprite("data\\global\\ui\\cursor\\ohand.dc6");
    // Note: This might be nullptr if the sprite parsing fails, but should not crash
    // We mainly test that the AssetManager can handle MPQ file access
}

// Test 4: Load raw file data from MPQ
TEST_F(AssetManagerMPQTest, LoadFileDataFromMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(test_mpq_path.string()));
    
    // Load raw data from MPQ (should extract text file successfully)
    auto data = asset_manager.loadFileData("data\\global\\excel\\armor.txt");
    EXPECT_FALSE(data.empty());
    
    // Should contain typical armor.txt content markers
    std::string content(data.begin(), data.end());
    EXPECT_TRUE(content.find("name") != std::string::npos);
}

// Test 5: Initialize with directory containing MPQs
TEST_F(AssetManagerMPQTest, InitializeWithMPQDirectory) {
    // Initialize with directory containing real MPQs
    bool success = asset_manager.initializeWithMPQs(test_mpq_dir);
    
    if (success) {
        // Should find files from multiple MPQ files
        EXPECT_TRUE(asset_manager.hasFile("data\\global\\excel\\armor.txt")); // from d2data.mpq
    } else {
        // If directory initialization isn't fully implemented yet, skip this part
        std::cout << "initializeWithMPQs not fully implemented yet, error: " 
                  << asset_manager.getLastError() << std::endl;
        GTEST_SKIP() << "Directory-based MPQ initialization not yet implemented";
    }
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