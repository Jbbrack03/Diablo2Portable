#include <gtest/gtest.h>
#include "tools/asset_extractor.h"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

namespace fs = std::filesystem;
using namespace d2;

class AssetExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        testD2Path = fs::temp_directory_path() / "test_d2_files";
        outputPath = fs::temp_directory_path() / "test_extracted_assets";
        
        fs::create_directories(testD2Path);
        fs::create_directories(outputPath);
        
        // Create mock MPQ files
        createMockMPQFile(testD2Path / "d2data.mpq");
        createMockMPQFile(testD2Path / "d2char.mpq");
        createMockMPQFile(testD2Path / "d2music.mpq");
    }
    
    void TearDown() override {
        fs::remove_all(testD2Path);
        fs::remove_all(outputPath);
    }
    
    void createMockMPQFile(const fs::path& path) {
        // Create a minimal MPQ file structure for testing
        std::ofstream file(path, std::ios::binary);
        // MPQ header signature
        const char signature[] = "MPQ\x1A";
        file.write(signature, 4);
        file.close();
    }
    
    fs::path testD2Path;
    fs::path outputPath;
};

TEST_F(AssetExtractorTest, ExtractAllGameAssets) {
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        testD2Path.string(),
        outputPath.string()
    );
    
    EXPECT_TRUE(result);
    
    // Verify output directories were created
    EXPECT_TRUE(fs::exists(outputPath / "sprites"));
    EXPECT_TRUE(fs::exists(outputPath / "sounds"));
    EXPECT_TRUE(fs::exists(outputPath / "data"));
    
    // Verify some files were extracted
    EXPECT_GT(extractor.getExtractedFileCount(), 0);
}

TEST_F(AssetExtractorTest, ExtractRealDC6Sprites) {
    AssetExtractor extractor;
    
    // Use real MPQ files from vendor directory for testing
    fs::path vendorMPQPath = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / 
                             "vendor" / "extracted_mpq";
    
    if (!fs::exists(vendorMPQPath)) {
        GTEST_SKIP() << "Vendor MPQ files not found at: " << vendorMPQPath;
    }
    
    bool result = extractor.extractFromD2(
        vendorMPQPath.string(),
        outputPath.string()
    );
    
    EXPECT_TRUE(result);
    
    // Check that actual DC6 sprites were extracted
    fs::path spritesPath = outputPath / "sprites";
    
    // Look for common DC6 files that should exist
    bool foundDC6Files = false;
    for (const auto& category : {"characters", "monsters", "items", "ui"}) {
        fs::path categoryPath = spritesPath / category;
        if (fs::exists(categoryPath)) {
            for (const auto& entry : fs::directory_iterator(categoryPath)) {
                if (entry.path().extension() == ".dc6") {
                    foundDC6Files = true;
                    break;
                }
            }
        }
    }
    
    EXPECT_TRUE(foundDC6Files) << "No DC6 files were extracted";
}

// Phase 33: Comprehensive Asset Extraction - Audio Extraction Test
TEST_F(AssetExtractorTest, ExtractAudioFiles) {
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        testD2Path.string(),
        outputPath.string()
    );
    
    EXPECT_TRUE(result);
    
    // Verify audio output directories were created with proper organization
    EXPECT_TRUE(fs::exists(outputPath / "sounds" / "music"));
    EXPECT_TRUE(fs::exists(outputPath / "sounds" / "effects"));
    EXPECT_TRUE(fs::exists(outputPath / "sounds" / "speech"));
    
    // Verify that audio extraction actually extracted WAV files
    // For now we test with mock data, but this will ensure the logic works
    EXPECT_GT(extractor.getExtractedAudioFileCount(), 0);
}

// Phase 33: Real WAV File Extraction Test
TEST_F(AssetExtractorTest, ExtractRealWAVFiles) {
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        testD2Path.string(),
        outputPath.string()
    );
    
    EXPECT_TRUE(result);
    
    // Verify that actual WAV files were extracted and organized by category
    fs::path soundsPath = outputPath / "sounds";
    
    // Check that WAV files exist in appropriate categories
    bool foundWAVFiles = false;
    for (const auto& category : {"music", "effects", "speech"}) {
        fs::path categoryPath = soundsPath / category;
        if (fs::exists(categoryPath)) {
            for (const auto& entry : fs::recursive_directory_iterator(categoryPath)) {
                if (entry.path().extension() == ".wav" || entry.path().extension() == ".WAV") {
                    foundWAVFiles = true;
                    break;
                }
            }
        }
    }
    
    EXPECT_TRUE(foundWAVFiles) << "No WAV files were extracted to organized directories";
}