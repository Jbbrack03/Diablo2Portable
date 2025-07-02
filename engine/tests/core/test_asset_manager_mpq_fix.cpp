#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include <filesystem>

using namespace d2portable::core;

TEST(AssetManagerMPQFixTest, InitializeWithUppercaseMPQExtensions) {
    AssetManager asset_manager;
    
    // The vendor directory has .MPQ files (uppercase)
    std::string mpq_dir = "/Users/jbbrack03/Diablo2Portable/vendor/mpq";
    
    // This should work with uppercase .MPQ extensions
    bool success = asset_manager.initializeWithMPQs(mpq_dir);
    
    EXPECT_TRUE(success) << "Failed to initialize with MPQ directory: " << asset_manager.getLastError();
    
    // If successful, should be able to find files
    if (success) {
        EXPECT_TRUE(asset_manager.hasFile("data\\global\\excel\\armor.txt"));
    }
}