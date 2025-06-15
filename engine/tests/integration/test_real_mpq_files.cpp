#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/mpq_loader.h"
#include <filesystem>
#include <iostream>

using namespace d2portable::core;
using namespace d2portable::utils;

// Integration tests for real Diablo II MPQ files
// These tests are disabled by default and only run when ENABLE_MPQ_INTEGRATION_TESTS is set
class RealMPQIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Skip tests if integration tests are not enabled
        const char* enable_tests = std::getenv("ENABLE_MPQ_INTEGRATION_TESTS");
        if (!enable_tests || std::string(enable_tests) != "1") {
            GTEST_SKIP() << "MPQ integration tests disabled. Set ENABLE_MPQ_INTEGRATION_TESTS=1 to run.";
        }
        
        // Look for common MPQ file locations
        std::vector<std::string> search_paths = {
            // Vendor directory (if MPQs are extracted)
            "vendor/mpq/",
            "vendor/Diablo II/",
            "vendor/d2data/",
            // Common installation paths
            "/Applications/Diablo II/",
            "C:/Program Files (x86)/Diablo II/",
            // User-specified path
            std::getenv("D2_DATA_PATH") ? std::getenv("D2_DATA_PATH") : ""
        };
        
        for (const auto& path : search_paths) {
            if (path.empty()) continue;
            
            if (std::filesystem::exists(path)) {
                data_path = path;
                // Look for d2data.mpq
                if (std::filesystem::exists(path + "d2data.mpq")) {
                    d2data_mpq = path + "d2data.mpq";
                    break;
                }
            }
        }
        
        if (d2data_mpq.empty()) {
            GTEST_SKIP() << "No Diablo II MPQ files found. Set D2_DATA_PATH environment variable.";
        }
    }
    
    std::string data_path;
    std::string d2data_mpq;
    AssetManager asset_manager;
};

// Test 1: Load real d2data.mpq
TEST_F(RealMPQIntegrationTest, LoadD2DataMPQ) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    std::cout << "Successfully loaded: " << d2data_mpq << std::endl;
}

// Test 2: Check for common files in d2data.mpq
TEST_F(RealMPQIntegrationTest, CheckCommonFiles) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // List of files that should exist in d2data.mpq
    std::vector<std::string> expected_files = {
        "data\\global\\ui\\cursor\\cursor.dc6",
        "data\\global\\ui\\fonts\\font8.dc6",
        "data\\global\\ui\\fonts\\font16.dc6",
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt",
        "data\\global\\excel\\misc.txt"
    };
    
    int found_count = 0;
    for (const auto& file : expected_files) {
        if (asset_manager.hasFile(file)) {
            found_count++;
            std::cout << "✓ Found: " << file << std::endl;
        } else {
            std::cout << "✗ Missing: " << file << std::endl;
        }
    }
    
    // Expect to find at least some files
    EXPECT_GT(found_count, 0) << "No expected files found in MPQ";
}

// Test 3: Extract and parse DC6 sprites
TEST_F(RealMPQIntegrationTest, ExtractDC6Sprites) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // Try to load cursor sprite
    auto cursor_sprite = asset_manager.loadSprite("data\\global\\ui\\cursor\\cursor.dc6");
    if (cursor_sprite) {
        std::cout << "Cursor sprite loaded successfully!" << std::endl;
        std::cout << "  Directions: " << cursor_sprite->getDirectionCount() << std::endl;
        std::cout << "  Frames per direction: " << cursor_sprite->getFramesPerDirection() << std::endl;
        
        EXPECT_GT(cursor_sprite->getDirectionCount(), 0);
        EXPECT_GT(cursor_sprite->getFramesPerDirection(), 0);
    } else {
        std::cout << "Failed to load cursor sprite" << std::endl;
    }
}

// Test 4: Extract text data files
TEST_F(RealMPQIntegrationTest, ExtractTextFiles) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // Try to load armor.txt
    auto armor_data = asset_manager.loadFileData("data\\global\\excel\\armor.txt");
    if (!armor_data.empty()) {
        std::cout << "armor.txt loaded: " << armor_data.size() << " bytes" << std::endl;
        
        // Check if it's text data
        bool is_text = true;
        for (size_t i = 0; i < std::min(size_t(100), armor_data.size()); i++) {
            if (armor_data[i] < 32 && armor_data[i] != '\t' && 
                armor_data[i] != '\n' && armor_data[i] != '\r') {
                is_text = false;
                break;
            }
        }
        
        if (is_text) {
            std::cout << "First 100 chars: ";
            for (size_t i = 0; i < std::min(size_t(100), armor_data.size()); i++) {
                std::cout << (char)armor_data[i];
            }
            std::cout << std::endl;
        }
        
        EXPECT_TRUE(is_text) << "armor.txt should be text data";
    }
}

// Test 5: Test compression handling
TEST_F(RealMPQIntegrationTest, TestCompressionTypes) {
    MPQLoader loader;
    ASSERT_TRUE(loader.open(d2data_mpq));
    
    auto files = loader.listFiles();
    std::cout << "Total files in MPQ: " << files.size() << std::endl;
    
    // Count compression types
    int uncompressed = 0;
    int compressed = 0;
    int encrypted = 0;
    
    for (const auto& file : files) {
        if (file.flags & 0x00000200) compressed++;  // MPQ_FILE_COMPRESS
        if (file.flags & 0x00010000) encrypted++;   // MPQ_FILE_ENCRYPTED
        if (!(file.flags & 0x00000200) && !(file.flags & 0x00010000)) uncompressed++;
    }
    
    std::cout << "Compression statistics:" << std::endl;
    std::cout << "  Uncompressed: " << uncompressed << std::endl;
    std::cout << "  Compressed: " << compressed << std::endl;
    std::cout << "  Encrypted: " << encrypted << std::endl;
    
    // Most D2 files are compressed
    EXPECT_GT(compressed, 0) << "Expected some compressed files";
}

// Test 6: Performance test - load multiple files
TEST_F(RealMPQIntegrationTest, PerformanceTest) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Load several sprites
    std::vector<std::string> sprite_files = {
        "data\\global\\ui\\cursor\\cursor.dc6",
        "data\\global\\ui\\fonts\\font8.dc6",
        "data\\global\\ui\\fonts\\font16.dc6",
        "data\\global\\ui\\panel\\invchar6.dc6"
    };
    
    int loaded = 0;
    for (const auto& file : sprite_files) {
        auto sprite = asset_manager.loadSprite(file);
        if (sprite) loaded++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Loaded " << loaded << " sprites in " << duration.count() << "ms" << std::endl;
    
    // Second load should be cached
    start = std::chrono::high_resolution_clock::now();
    
    for (const auto& file : sprite_files) {
        auto sprite = asset_manager.loadSprite(file);
        if (sprite) loaded++;
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto cached_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Cached load took " << cached_duration.count() << "ms" << std::endl;
    
    // Cached should be much faster
    EXPECT_LT(cached_duration.count(), duration.count() / 2) << "Cached load should be faster";
}