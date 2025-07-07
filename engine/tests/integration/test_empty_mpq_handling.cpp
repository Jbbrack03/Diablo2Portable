#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/stormlib_mpq_loader.h"
#include <filesystem>
#include <fstream>

using namespace d2portable::core;
using namespace d2portable::utils;

// Test suite for handling empty MPQ files gracefully
class EmptyMPQHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary empty file to simulate empty MPQ
        test_mpq_path = "test_empty.mpq";
        std::ofstream empty_file(test_mpq_path, std::ios::binary);
        empty_file.close();
    }
    
    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(test_mpq_path)) {
            std::filesystem::remove(test_mpq_path);
        }
    }
    
    std::string test_mpq_path;
    AssetManager asset_manager;
};

// Test 1: AssetManager should fail gracefully on empty MPQ
TEST_F(EmptyMPQHandlingTest, AssetManagerHandlesEmptyMPQ) {
    // Initialize with empty MPQ should fail gracefully
    EXPECT_FALSE(asset_manager.initializeWithMPQ(test_mpq_path));
    
    // Asset manager should still be usable after failed initialization
    EXPECT_FALSE(asset_manager.hasFile("test.txt"));
}