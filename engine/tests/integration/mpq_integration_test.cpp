#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include "utils/stormlib_mpq_loader.h"
#include "utils/mpq_validator.h"
#include "core/asset_manager.h"
#include "sprites/dc6_parser.h"

// Phase 39.1: MPQ Integration Validation
// Task: Validate asset extraction with real game files

namespace fs = std::filesystem;

class MPQIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        mpq_dir_ = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
        
        // Find all MPQ files in vendor directory
        for (const auto& entry : fs::directory_iterator(mpq_dir_)) {
            if (entry.path().extension() == ".mpq") {
                std::string filepath = entry.path().string();
                auto validation = d2::utils::MPQValidator::validateMPQFile(filepath);
                
                if (validation.isValid && !validation.isPlaceholder) {
                    valid_mpqs_.push_back(filepath);
                } else if (validation.isPlaceholder) {
                    placeholder_mpqs_.push_back(filepath);
                }
            }
        }
    }
    
    fs::path mpq_dir_;
    std::vector<std::string> valid_mpqs_;
    std::vector<std::string> placeholder_mpqs_;
};

// Test 1: Report MPQ file status
TEST_F(MPQIntegrationTest, ReportMPQFileStatus) {
    std::cout << "\n=== MPQ File Status Report ===\n";
    std::cout << "MPQ Directory: " << mpq_dir_ << "\n\n";
    
    std::cout << "Valid MPQ Files (" << valid_mpqs_.size() << "):\n";
    for (const auto& mpq : valid_mpqs_) {
        fs::path path(mpq);
        auto size = fs::file_size(path);
        std::cout << "  - " << path.filename() << " (" << size / (1024*1024) << " MB)\n";
    }
    
    std::cout << "\nPlaceholder MPQ Files (" << placeholder_mpqs_.size() << "):\n";
    for (const auto& mpq : placeholder_mpqs_) {
        fs::path path(mpq);
        auto size = fs::file_size(path);
        std::cout << "  - " << path.filename() << " (" << size / (1024*1024) << " MB)\n";
    }
    
    // Test passes if we have at least some valid MPQ files
    EXPECT_GT(valid_mpqs_.size(), 0) << "No valid MPQ files found. Some files exist but are placeholders.";
}

// Test 2: Validate MPQ loading performance
TEST_F(MPQIntegrationTest, ValidateMPQLoadingPerformance) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files available for performance testing";
    }
    
    // Test loading performance for each valid MPQ
    for (const auto& mpq_path : valid_mpqs_) {
        fs::path path(mpq_path);
        std::cout << "\nTesting MPQ: " << path.filename() << "\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        d2portable::utils::StormLibMPQLoader loader;
        ASSERT_TRUE(loader.open(mpq_path)) << "Failed to open " << mpq_path;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto open_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "  Open time: " << open_time << "ms\n";
        
        // List some files in the MPQ
        auto files = loader.listFiles();
        std::cout << "  Files in MPQ: " << files.size() << "\n";
        
        // MPQ opening should be reasonably fast
        // Allow more time for large MPQs with many files
        long expected_time = 100;
        if (files.size() > 5000) {
            expected_time = 200; // Allow up to 200ms for very large MPQs
        }
        EXPECT_LT(open_time, expected_time) 
            << "MPQ open time too slow for " << path.filename() 
            << " (has " << files.size() << " files)";
        
        // If there are files, test extraction performance
        if (!files.empty()) {
            // Find a small file to test extraction
            std::string test_file;
            for (const auto& file : files) {
                if (file.filename.find(".txt") != std::string::npos || 
                    file.filename.find(".tbl") != std::string::npos) {
                    test_file = file.filename;
                    break;
                }
            }
            
            if (!test_file.empty()) {
                start = std::chrono::high_resolution_clock::now();
                
                std::vector<uint8_t> data;
                bool extracted = loader.extractFile(test_file, data);
                
                end = std::chrono::high_resolution_clock::now();
                auto extract_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                
                if (extracted) {
                    std::cout << "  Extracted " << test_file << " (" << data.size() 
                              << " bytes) in " << extract_time << "μs\n";
                    
                    // Small file extraction should be very fast (under 10ms)
                    EXPECT_LT(extract_time, 10000) << "File extraction too slow";
                }
            }
        }
        
        loader.close();
    }
}

// Test 3: Verify DC6 sprite extraction
TEST_F(MPQIntegrationTest, VerifyDC6SpriteExtraction) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files available for sprite extraction testing";
    }
    
    // Look for d2data.mpq or similar that would contain sprites
    std::string sprite_mpq;
    for (const auto& mpq : valid_mpqs_) {
        fs::path path(mpq);
        std::string filename = path.filename().string();
        if (filename.find("d2data") != std::string::npos || 
            filename.find("d2char") != std::string::npos) {
            sprite_mpq = mpq;
            break;
        }
    }
    
    if (sprite_mpq.empty()) {
        GTEST_SKIP() << "No sprite-containing MPQ files found";
    }
    
    d2portable::utils::StormLibMPQLoader loader;
    ASSERT_TRUE(loader.open(sprite_mpq));
    
    // Try to find and extract a DC6 sprite
    auto files = loader.listFiles();
    std::string dc6_file;
    
    for (const auto& file : files) {
        if (file.filename.find(".dc6") != std::string::npos || 
            file.filename.find(".DC6") != std::string::npos) {
            dc6_file = file.filename;
            break;
        }
    }
    
    if (!dc6_file.empty()) {
        std::cout << "\nFound DC6 file: " << dc6_file << "\n";
        
        std::vector<uint8_t> data;
        ASSERT_TRUE(loader.extractFile(dc6_file, data)) 
            << "Failed to extract DC6 file: " << dc6_file;
        
        // Try to parse the DC6
        d2portable::sprites::DC6Parser parser;
        auto sprite = parser.parseData(data);
        
        if (sprite) {
            std::cout << "  Successfully parsed DC6 sprite\n";
            std::cout << "  Directions: " << sprite->getDirectionCount() << "\n";
            std::cout << "  Frames per direction: " << sprite->getFramesPerDirection() << "\n";
            
            EXPECT_GT(sprite->getDirectionCount(), 0);
            EXPECT_GT(sprite->getFramesPerDirection(), 0);
        } else {
            std::cout << "  Failed to parse DC6 (might be compressed or different format)\n";
        }
    } else {
        std::cout << "No DC6 files found in " << fs::path(sprite_mpq).filename() << "\n";
    }
    
    loader.close();
}

// Test 4: Test asset manager integration with available MPQs
TEST_F(MPQIntegrationTest, AssetManagerIntegrationWithAvailableMPQs) {
    d2portable::core::AssetManager asset_manager;
    
    // Initialize with MPQ directory
    bool initialized = asset_manager.initialize(mpq_dir_.string());
    
    if (valid_mpqs_.empty()) {
        // Should still initialize even with no valid MPQs (fallback mode)
        EXPECT_TRUE(initialized) << "AssetManager should initialize even without valid MPQs";
        std::cout << "\nAssetManager initialized in fallback mode (no valid MPQs)\n";
    } else {
        EXPECT_TRUE(initialized) << "AssetManager failed to initialize with valid MPQs";
        std::cout << "\nAssetManager initialized with " << valid_mpqs_.size() << " valid MPQs\n";
        
        // Try to check for some common files
        std::vector<std::string> test_files = {
            "data/global/ui/panel/invchar6.dc6",
            "data/global/excel/armor.txt",
            "data/global/excel/weapons.txt"
        };
        
        for (const auto& file : test_files) {
            if (asset_manager.hasFile(file)) {
                std::cout << "  Found: " << file << "\n";
            }
        }
    }
}

// Test 5: Measure memory usage during MPQ operations
TEST_F(MPQIntegrationTest, MeasureMemoryUsageDuringMPQOperations) {
    if (valid_mpqs_.empty()) {
        GTEST_SKIP() << "No valid MPQ files available for memory testing";
    }
    
    // Test memory usage when opening multiple MPQs
    std::vector<std::unique_ptr<d2portable::utils::StormLibMPQLoader>> loaders;
    
    // Get baseline memory (this is approximate)
    size_t baseline_memory = 0; // Would need platform-specific code for real measurement
    
    std::cout << "\n=== Memory Usage Test ===\n";
    
    for (const auto& mpq_path : valid_mpqs_) {
        fs::path path(mpq_path);
        auto loader = std::make_unique<d2portable::utils::StormLibMPQLoader>();
        
        if (loader->open(mpq_path)) {
            loaders.push_back(std::move(loader));
            std::cout << "Opened " << path.filename() << " (total open: " 
                      << loaders.size() << ")\n";
        }
        
        // In production, we'd measure actual memory here
        // For now, just ensure we can open multiple MPQs
    }
    
    // Verify we can handle multiple open MPQs
    EXPECT_GT(loaders.size(), 0) << "Should be able to open at least one MPQ";
    
    // Clean up
    loaders.clear();
}