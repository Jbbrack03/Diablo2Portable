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
    // Check if we have real MPQ files in vendor/mpq
    fs::path vendorMPQPath = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
    bool hasRealMPQs = false;
    
    if (fs::exists(vendorMPQPath)) {
        // Check for non-placeholder d2data.mpq
        fs::path d2dataPath = vendorMPQPath / "d2data.mpq";
        if (fs::exists(d2dataPath) && fs::file_size(d2dataPath) > 10 * 1024 * 1024) {
            hasRealMPQs = true;
        }
    }
    
    if (!hasRealMPQs) {
        GTEST_SKIP() << "Skipping asset extraction test - real MPQ files not available. "
                     << "Place valid Diablo II MPQ files in vendor/mpq/ to enable this test.";
    }
    
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        vendorMPQPath.string(),
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

// ExtractRealDC6Sprites test removed - redundant with ExtractAllGameAssets
// and was looking for non-existent vendor/extracted_mpq directory

// Phase 33: Comprehensive Asset Extraction - Audio Extraction Test
TEST_F(AssetExtractorTest, ExtractAudioFiles) {
    // Check for real MPQ files
    fs::path vendorMPQPath = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
    fs::path d2musicPath = vendorMPQPath / "d2music.mpq";
    
    if (!fs::exists(d2musicPath) || fs::file_size(d2musicPath) < 10 * 1024 * 1024) {
        GTEST_SKIP() << "Skipping audio extraction test - d2music.mpq not available or is placeholder";
    }
    
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        vendorMPQPath.string(),
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
    // Check for real MPQ files
    fs::path vendorMPQPath = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
    fs::path d2musicPath = vendorMPQPath / "d2music.mpq";
    
    if (!fs::exists(d2musicPath) || fs::file_size(d2musicPath) < 10 * 1024 * 1024) {
        GTEST_SKIP() << "Skipping WAV extraction test - d2music.mpq not available or is placeholder";
    }
    
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        vendorMPQPath.string(),
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

// Phase 33: Data Table Extraction Test
TEST_F(AssetExtractorTest, ExtractDataTables) {
    // Check for real MPQ files  
    fs::path vendorMPQPath = fs::path(__FILE__).parent_path().parent_path().parent_path().parent_path() / "vendor" / "mpq";
    fs::path d2dataPath = vendorMPQPath / "d2data.mpq";
    
    if (!fs::exists(d2dataPath) || fs::file_size(d2dataPath) < 10 * 1024 * 1024) {
        GTEST_SKIP() << "Skipping data table extraction test - d2data.mpq not available or is placeholder";
    }
    
    AssetExtractor extractor;
    
    bool result = extractor.extractFromD2(
        vendorMPQPath.string(),
        outputPath.string()
    );
    
    EXPECT_TRUE(result);
    
    // Verify data output directories were created with proper organization
    EXPECT_TRUE(fs::exists(outputPath / "data" / "excel"));
    EXPECT_TRUE(fs::exists(outputPath / "data" / "string_tables"));
    EXPECT_TRUE(fs::exists(outputPath / "data" / "binary"));
    
    // Verify that actual data files were extracted
    EXPECT_GT(extractor.getExtractedDataFileCount(), 0);
    
    // Check that data files exist in appropriate categories  
    fs::path dataPath = outputPath / "data";
    
    bool foundDataFiles = false;
    for (const auto& category : {"excel", "string_tables", "binary"}) {
        fs::path categoryPath = dataPath / category;
        if (fs::exists(categoryPath)) {
            for (const auto& entry : fs::recursive_directory_iterator(categoryPath)) {
                if (entry.is_regular_file()) {
                    foundDataFiles = true;
                    break;
                }
            }
        }
    }
    
    EXPECT_TRUE(foundDataFiles) << "No data files were extracted to organized directories";
}