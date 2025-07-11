#include <gtest/gtest.h>
#include "utils/mpq_validator.h"
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using namespace d2::utils;

// Test validation of actual vendor MPQ files
TEST(MPQValidatorVendorTest, IdentifyPlaceholderFilesInVendor) {
    std::string vendorPath = "vendor/mpq/";
    
    // Known placeholder files based on our investigation
    std::vector<std::string> expectedPlaceholders = {
        vendorPath + "d2data.mpq",
        vendorPath + "d2exp.mpq", 
        vendorPath + "d2sfx.mpq"
    };
    
    // Validate each expected placeholder
    for (const auto& file : expectedPlaceholders) {
        if (fs::exists(file)) {
            auto result = MPQValidator::validateMPQFile(file);
            EXPECT_FALSE(result.isValid) << "File should not be valid: " << file;
            EXPECT_TRUE(result.isPlaceholder) << "File should be placeholder: " << file;
            EXPECT_EQ(result.fileSize, 1048576) << "Placeholder should be 1MB: " << file;
        }
    }
}

TEST(MPQValidatorVendorTest, IdentifyAuthenticFilesInVendor) {
    std::string vendorPath = "vendor/mpq/";
    
    // Known authentic files based on our investigation
    std::vector<std::string> expectedAuthentic = {
        vendorPath + "d2char.mpq",
        vendorPath + "d2music.mpq",
        vendorPath + "d2speech.mpq",
        vendorPath + "d2video.mpq",
        vendorPath + "d2xmusic.mpq",
        vendorPath + "d2xtalk.mpq",
        vendorPath + "d2xvideo.mpq",
        vendorPath + "playd2.mpq",
        vendorPath + "setup.mpq"
    };
    
    // Validate each expected authentic file
    for (const auto& file : expectedAuthentic) {
        if (fs::exists(file)) {
            auto result = MPQValidator::validateMPQFile(file);
            EXPECT_TRUE(result.isValid) << "File should be valid: " << file;
            EXPECT_FALSE(result.isPlaceholder) << "File should not be placeholder: " << file;
            EXPECT_TRUE(result.error.empty()) << "Should have no error: " << file;
        }
    }
}

TEST(MPQValidatorVendorTest, ValidateAllVendorMPQFiles) {
    std::string vendorPath = "vendor/mpq/";
    std::vector<std::string> allFiles;
    
    // Collect all MPQ files
    if (fs::exists(vendorPath)) {
        for (const auto& entry : fs::directory_iterator(vendorPath)) {
            if (entry.path().extension() == ".mpq" || entry.path().extension() == ".MPQ") {
                allFiles.push_back(entry.path().string());
            }
        }
    }
    
    auto placeholders = MPQValidator::getPlaceholderFiles(allFiles);
    auto authentic = MPQValidator::getAuthenticFiles(allFiles);
    
    // We should have exactly 3 placeholder files
    EXPECT_EQ(placeholders.size(), 3) << "Should have exactly 3 placeholder files";
    
    // We should have exactly 9 authentic files 
    EXPECT_EQ(authentic.size(), 9) << "Should have exactly 9 authentic files";
    
    // Total should be 12 MPQ files
    EXPECT_EQ(allFiles.size(), 12) << "Should have exactly 12 MPQ files total";
}