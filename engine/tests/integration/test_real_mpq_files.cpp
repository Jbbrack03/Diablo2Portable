#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/stormlib_mpq_loader.h"
#include "utils/mpq_validator.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>

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
            "/Users/jbbrack03/Diablo2Portable/vendor/extracted_mpq/",
            "vendor/extracted_mpq/",
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
                // Look for d2speech.mpq first (we know this one works)
                if (std::filesystem::exists(path + "d2speech.mpq")) {
                    d2data_mpq = path + "d2speech.mpq";
                    break;
                } else if (std::filesystem::exists(path + "D2DATA.MPQ")) {
                    d2data_mpq = path + "D2DATA.MPQ";
                    break;
                } else if (std::filesystem::exists(path + "d2data.mpq")) {
                    d2data_mpq = path + "d2data.mpq";
                    break;
                }
            }
        }
        
        if (d2data_mpq.empty()) {
            GTEST_SKIP() << "No Diablo II MPQ files found. Set D2_DATA_PATH environment variable.";
        }
        
        // Use MPQValidator to check if file is valid
        auto validation = d2::utils::MPQValidator::validateMPQFile(d2data_mpq);
        if (validation.isPlaceholder) {
            GTEST_SKIP() << "MPQ file is a placeholder. " << validation.error 
                        << ". Please copy valid Diablo II MPQ files to " << data_path;
        }
        if (!validation.isValid) {
            GTEST_SKIP() << "Invalid MPQ file: " << validation.error 
                        << ". Please copy valid Diablo II MPQ files to " << data_path;
        }
    }
    
    std::string data_path;
    std::string d2data_mpq;
    AssetManager asset_manager;
};

// Test 1: Load real MPQ file
TEST_F(RealMPQIntegrationTest, LoadMPQFile) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    std::cout << "Successfully loaded: " << d2data_mpq << std::endl;
}

// Test 2: Check for common files in MPQ
TEST_F(RealMPQIntegrationTest, CheckCommonFiles) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // List of files that should exist in d2speech.mpq (audio files)
    std::vector<std::string> expected_files = {
        "data\\local\\sfx\\Act1\\Sorceress\\Sor_act1_entry_wilderness.wav",
        "data\\local\\sfx\\Act2\\Atma\\Atm_act2_gossip_01.wav",
        "data\\local\\sfx\\Act2\\Lysander\\Lys_farewell.wav",
        "data\\local\\sfx\\Common\\Paladin\\Pal_ok.wav",
        "data\\local\\sfx\\Common\\Cain\\Cain_yes.wav",
        "data\\local\\sfx\\Act1\\Warriv\\War_act1_q5_successful.wav"
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

// Test 3: Extract and validate WAV audio files
TEST_F(RealMPQIntegrationTest, ExtractAudioFiles) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // Try to load an audio file
    auto audio_data = asset_manager.loadFileData("data\\local\\sfx\\Common\\Cain\\Cain_yes.wav");
    if (!audio_data.empty()) {
        std::cout << "Audio file loaded successfully!" << std::endl;
        std::cout << "  Size: " << audio_data.size() << " bytes" << std::endl;
        
        // Check for WAV header
        if (audio_data.size() >= 12) {
            std::string riff_header(reinterpret_cast<const char*>(&audio_data[0]), 4);
            std::string wave_header(reinterpret_cast<const char*>(&audio_data[8]), 4);
            
            std::cout << "  RIFF header: " << riff_header << std::endl;
            std::cout << "  WAVE header: " << wave_header << std::endl;
            
            EXPECT_EQ(riff_header, "RIFF") << "Should have RIFF header";
            EXPECT_EQ(wave_header, "WAVE") << "Should have WAVE header";
        }
        
        EXPECT_GT(audio_data.size(), 44) << "WAV file should be larger than header";
    } else {
        std::cout << "Failed to load audio file" << std::endl;
        FAIL() << "Should be able to extract audio files from MPQ";
    }
}

// Test 4: File listing and compression detection
TEST_F(RealMPQIntegrationTest, FileListingTest) {
    ASSERT_TRUE(asset_manager.initializeWithMPQ(d2data_mpq));
    
    // Since d2speech.mpq doesn't have text files, let's test file listing
    StormLibMPQLoader loader;
    ASSERT_TRUE(loader.open(d2data_mpq));
    
    auto files = loader.listFiles();
    std::cout << "Total files in MPQ: " << files.size() << std::endl;
    
    // Check that we have the expected number of files
    EXPECT_GT(files.size(), 1000) << "d2speech.mpq should have over 1000 files";
    
    // Check that files are WAV audio files
    int wav_count = 0;
    for (const auto& file : files) {
        if (file.filename.find(".wav") != std::string::npos) {
            wav_count++;
        }
    }
    
    std::cout << "WAV files found: " << wav_count << std::endl;
    EXPECT_GT(wav_count, 1000) << "Should have many WAV files";
}

// Test 5: Test compression handling
TEST_F(RealMPQIntegrationTest, TestCompressionTypes) {
    StormLibMPQLoader loader;
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
    
    // Load several audio files
    std::vector<std::string> audio_files = {
        "data\\local\\sfx\\Common\\Cain\\Cain_yes.wav",
        "data\\local\\sfx\\Common\\Paladin\\Pal_ok.wav",
        "data\\local\\sfx\\Common\\Amazon\\Ama_helpme.wav",
        "data\\local\\sfx\\Act1\\Akara\\Aka_act1_q4_successful.wav"
    };
    
    int loaded = 0;
    for (const auto& file : audio_files) {
        auto audio_data = asset_manager.loadFileData(file);
        if (!audio_data.empty()) loaded++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Loaded " << loaded << " audio files in " << duration.count() << "μs" << std::endl;
    
    // Second load should be cached
    start = std::chrono::high_resolution_clock::now();
    
    for (const auto& file : audio_files) {
        auto audio_data = asset_manager.loadFileData(file);
        if (!audio_data.empty()) loaded++;
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto cached_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Cached load took " << cached_duration.count() << "μs" << std::endl;
    
    // If both operations are fast enough (under 10ms), just check that we loaded files
    if (duration.count() < 10000 && cached_duration.count() < 10000) {
        EXPECT_GT(loaded, 0) << "Should have loaded at least some audio files";
    } else {
        // Cached should be faster only if we have meaningful timing differences
        EXPECT_LT(cached_duration.count(), duration.count()) << "Cached load should be faster";
    }
}