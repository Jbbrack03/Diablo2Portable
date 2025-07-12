#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "onboarding/asset_validator.h"
#include "onboarding/file_source_detector.h" // For D2Version enum

namespace fs = std::filesystem;
using namespace d2;

class AssetValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "asset_validator_test";
        fs::create_directories(testDir);
    }
    
    void TearDown() override {
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    // Create mock MPQ files with basic structure
    void createMockMPQFiles(const fs::path& dir, bool includeExpansion = true) {
        fs::create_directories(dir);
        
        // Core MPQ files
        std::vector<std::string> coreFiles = {
            "d2data.mpq",
            "d2sfx.mpq",
            "d2speech.mpq"
        };
        
        for (const auto& filename : coreFiles) {
            std::ofstream file(dir / filename, std::ios::binary);
            // Write MPQ header signature
            char mpq_header[] = {'M', 'P', 'Q', 0x1A};
            file.write(mpq_header, sizeof(mpq_header));
            // Add some dummy data
            std::vector<char> data(1024, 0);
            file.write(data.data(), data.size());
        }
        
        if (includeExpansion) {
            std::ofstream file(dir / "d2exp.mpq", std::ios::binary);
            char mpq_header[] = {'M', 'P', 'Q', 0x1A};
            file.write(mpq_header, sizeof(mpq_header));
            std::vector<char> data(1024, 0);
            file.write(data.data(), data.size());
        }
    }
    
    fs::path testDir;
};

// STEP 1: Write exactly ONE failing test for validating asset completeness
TEST_F(AssetValidatorTest, ValidateCompleteness) {
    AssetValidator validator;
    fs::path assetPath = testDir / "assets";
    
    // Create complete set of mock MPQ files
    createMockMPQFiles(assetPath, true);
    
    auto validation = validator.validateAssets(assetPath.string());
    
    EXPECT_TRUE(validation.isComplete);
    EXPECT_EQ(validation.missingFiles.size(), 0);
    EXPECT_TRUE(validation.hasExpansion);
    EXPECT_GE(validation.version, D2Version::CLASSIC);
}

// STEP 2: Write exactly ONE failing test for detecting corrupted files
TEST_F(AssetValidatorTest, DetectCorruption) {
    AssetValidator validator;
    
    // Create a corrupted MPQ file (invalid header)
    fs::path corruptedMPQ = testDir / "corrupted.mpq";
    std::ofstream file(corruptedMPQ, std::ios::binary);
    // Write invalid header
    char bad_header[] = {'B', 'A', 'D', 0x00};
    file.write(bad_header, sizeof(bad_header));
    std::vector<char> data(1024, 0xFF);
    file.write(data.data(), data.size());
    file.close();
    
    bool isCorrupted = validator.detectCorruption(corruptedMPQ.string());
    
    EXPECT_TRUE(isCorrupted);
    
    // Test with valid MPQ
    fs::path validMPQ = testDir / "valid.mpq";
    std::ofstream validFile(validMPQ, std::ios::binary);
    char mpq_header[] = {'M', 'P', 'Q', 0x1A};
    validFile.write(mpq_header, sizeof(mpq_header));
    validFile.write(data.data(), data.size());
    validFile.close();
    
    bool isValid = validator.detectCorruption(validMPQ.string());
    
    EXPECT_FALSE(isValid); // Should return false for valid files (not corrupted)
}

// STEP 3: Write exactly ONE failing test for detecting incomplete assets
TEST_F(AssetValidatorTest, DetectIncompleteAssets) {
    AssetValidator validator;
    fs::path incompleteAssetPath = testDir / "incomplete_assets";
    
    // Create only core files, missing expansion files
    createMockMPQFiles(incompleteAssetPath, false); // No expansion
    
    auto validation = validator.validateAssets(incompleteAssetPath.string());
    
    EXPECT_FALSE(validation.isComplete);
    EXPECT_FALSE(validation.hasExpansion);
    EXPECT_GT(validation.missingFiles.size(), 0);
    EXPECT_TRUE(std::find(validation.missingFiles.begin(), validation.missingFiles.end(), "d2exp.mpq") != validation.missingFiles.end());
}

// STEP 4: Write exactly ONE failing test for checksum verification
TEST_F(AssetValidatorTest, VerifyFileChecksum) {
    AssetValidator validator;
    
    // Create a test file with known content
    fs::path testFile = testDir / "test_file.txt";
    std::string testContent = "This is test content for checksum verification";
    std::ofstream file(testFile);
    file << testContent;
    file.close();
    
    // Compute the actual checksum for the test content
    std::string actualChecksum = validator.computeChecksum(testFile.string());
    EXPECT_FALSE(actualChecksum.empty()); // Should compute a valid checksum
    
    // Now verify with the correct checksum
    bool isValid = validator.verifyChecksum(testFile.string(), actualChecksum);
    EXPECT_TRUE(isValid); // Should match when using the correct checksum
    
    // Test with wrong checksum
    bool wrongChecksumResult = validator.verifyChecksum(testFile.string(), "wrong_checksum");
    EXPECT_FALSE(wrongChecksumResult); // Should fail with wrong checksum
    
    // Test with non-existent file
    bool nonExistentResult = validator.verifyChecksum("/nonexistent/file.txt", "any_checksum");
    EXPECT_FALSE(nonExistentResult); // Should be false for non-existent files
}