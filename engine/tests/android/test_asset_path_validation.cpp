#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include "android/asset_path_validator.h"

namespace fs = std::filesystem;

class AssetPathValidationTest : public ::testing::Test {
protected:
    std::string test_asset_path;
    
    void SetUp() override {
        // Create a temporary test directory
        test_asset_path = "test_assets_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed());
        fs::create_directories(test_asset_path);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (fs::exists(test_asset_path)) {
            fs::remove_all(test_asset_path);
        }
    }
    
    void createMockMPQFile(const std::string& filename) {
        std::ofstream file(fs::path(test_asset_path) / filename, std::ios::binary);
        // MPQ header signature
        file.write("MPQ\x1A", 4);
        // Minimal valid MPQ header
        uint32_t header_size = 32;
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        uint32_t archive_size = 1024;
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        // Fill rest with zeros
        std::vector<char> zeros(1012, 0);
        file.write(zeros.data(), zeros.size());
    }
};

TEST_F(AssetPathValidationTest, ValidateAssetPathContainsMPQFiles) {
    // Create mock MPQ files
    createMockMPQFile("d2data.mpq");
    createMockMPQFile("d2exp.mpq");
    
    // Use the asset path validator
    bool hasRequiredMPQs = d2::AssetPathValidator::hasRequiredMPQFiles(test_asset_path);
    
    EXPECT_TRUE(hasRequiredMPQs);
}

TEST_F(AssetPathValidationTest, DetectsMissingRequiredFiles) {
    // Only create one required file
    createMockMPQFile("d2data.mpq");
    // Missing d2exp.mpq
    
    auto result = d2::AssetPathValidator::validateAssetPath(test_asset_path);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_EQ(result.missingFiles.size(), 1);
    EXPECT_EQ(result.missingFiles[0], "d2exp.mpq");
}

TEST_F(AssetPathValidationTest, ValidatesOptionalFiles) {
    // Create required files
    createMockMPQFile("d2data.mpq");
    createMockMPQFile("d2exp.mpq");
    
    // Create some optional files
    createMockMPQFile("d2music.mpq");
    createMockMPQFile("d2sfx.mpq");
    
    auto mpqFiles = d2::AssetPathValidator::findMPQFiles(test_asset_path);
    
    EXPECT_EQ(mpqFiles.size(), 4);
    EXPECT_TRUE(std::find(mpqFiles.begin(), mpqFiles.end(), "d2data.mpq") != mpqFiles.end());
    EXPECT_TRUE(std::find(mpqFiles.begin(), mpqFiles.end(), "d2exp.mpq") != mpqFiles.end());
    EXPECT_TRUE(std::find(mpqFiles.begin(), mpqFiles.end(), "d2music.mpq") != mpqFiles.end());
    EXPECT_TRUE(std::find(mpqFiles.begin(), mpqFiles.end(), "d2sfx.mpq") != mpqFiles.end());
}

TEST_F(AssetPathValidationTest, RejectsInvalidMPQFiles) {
    // Create an invalid MPQ file (too small)
    std::ofstream file(fs::path(test_asset_path) / "invalid.mpq", std::ios::binary);
    file.write("MPQ", 3); // Too short
    file.close();
    
    auto mpqFiles = d2::AssetPathValidator::findMPQFiles(test_asset_path);
    
    EXPECT_EQ(mpqFiles.size(), 0);
}

TEST_F(AssetPathValidationTest, HandlesNonExistentPath) {
    std::string nonExistentPath = "non_existent_path_12345";
    
    auto result = d2::AssetPathValidator::validateAssetPath(nonExistentPath);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errorMessage.empty());
    EXPECT_TRUE(result.errorMessage.find("does not exist") != std::string::npos);
}