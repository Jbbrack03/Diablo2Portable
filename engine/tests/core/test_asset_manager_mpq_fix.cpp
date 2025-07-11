#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include <filesystem>
#include <fstream>

using namespace d2portable::core;

TEST(AssetManagerMPQFixTest, InitializeWithUppercaseMPQExtensions) {
    AssetManager asset_manager;
    
    // Get the path relative to the test executable
    std::filesystem::path exe_dir = std::filesystem::path(__FILE__).parent_path();
    std::filesystem::path project_root = exe_dir.parent_path().parent_path().parent_path();
    std::string mpq_dir = (project_root / "vendor" / "mpq").string();
    
    // First check if we have the critical d2data.mpq file
    std::string d2data_path = mpq_dir + "/d2data.mpq";
    bool has_valid_d2data = false;
    
    if (std::filesystem::exists(d2data_path)) {
        auto file_size = std::filesystem::file_size(d2data_path);
        if (file_size > 0) {
            // Check MPQ header
            std::ifstream file(d2data_path, std::ios::binary);
            if (file.is_open()) {
                uint32_t signature;
                file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
                if (signature == 0x1A51504D) { // 'MPQ\x1A'
                    has_valid_d2data = true;
                }
            }
        }
    }
    
    if (!has_valid_d2data) {
        GTEST_SKIP() << "Valid d2data.mpq not found in " << mpq_dir << ". This file is required for armor.txt. Please copy valid Diablo II MPQ files.";
    }
    
    // This should work with uppercase .MPQ extensions
    bool success = asset_manager.initializeWithMPQs(mpq_dir);
    
    EXPECT_TRUE(success) << "Failed to initialize with MPQ directory: " << asset_manager.getLastError();
    
    // If successful, should be able to find files
    if (success) {
        EXPECT_TRUE(asset_manager.hasFile("data\\global\\excel\\armor.txt"));
    }
}