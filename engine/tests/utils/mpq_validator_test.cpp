#include <gtest/gtest.h>
#include "utils/mpq_validator.h"
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace d2::utils;

// Test MPQ file validation
TEST(MPQValidatorTest, ValidatePlaceholderFile) {
    // Create a test placeholder file
    std::string testFile = "test_placeholder.mpq";
    std::ofstream out(testFile, std::ios::binary);
    for (int i = 0; i < 1024; i++) {
        out << 'X';
    }
    out.close();
    
    auto result = MPQValidator::validateMPQFile(testFile);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_TRUE(result.isPlaceholder);
    EXPECT_EQ(result.error, "File is a placeholder (filled with 'X' characters)");
    EXPECT_EQ(result.fileSize, 1024);
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST(MPQValidatorTest, ValidateAuthenticFile) {
    // Create a test authentic MPQ file
    std::string testFile = "test_authentic.mpq";
    std::ofstream out(testFile, std::ios::binary);
    out.write("MPQ\x1A", 4);
    // Write some dummy data
    for (int i = 0; i < 100; i++) {
        out << static_cast<char>(i);
    }
    out.close();
    
    auto result = MPQValidator::validateMPQFile(testFile);
    
    EXPECT_TRUE(result.isValid);
    EXPECT_FALSE(result.isPlaceholder);
    EXPECT_TRUE(result.error.empty());
    EXPECT_EQ(result.fileSize, 104);
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST(MPQValidatorTest, ValidateInvalidFile) {
    // Create a test invalid file
    std::string testFile = "test_invalid.mpq";
    std::ofstream out(testFile, std::ios::binary);
    out.write("ABCD", 4);
    out.close();
    
    auto result = MPQValidator::validateMPQFile(testFile);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.isPlaceholder);
    EXPECT_EQ(result.error, "Invalid MPQ header");
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST(MPQValidatorTest, ValidateNonExistentFile) {
    auto result = MPQValidator::validateMPQFile("non_existent_file.mpq");
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.isPlaceholder);
    EXPECT_EQ(result.error, "Failed to open file");
}

TEST(MPQValidatorTest, GetPlaceholderFiles) {
    // Create test files
    std::vector<std::string> testFiles = {
        "test_placeholder1.mpq",
        "test_placeholder2.mpq",
        "test_authentic1.mpq"
    };
    
    // Create placeholder files
    for (int i = 0; i < 2; i++) {
        std::ofstream out(testFiles[i], std::ios::binary);
        for (int j = 0; j < 100; j++) {
            out << 'X';
        }
        out.close();
    }
    
    // Create authentic file
    std::ofstream out(testFiles[2], std::ios::binary);
    out.write("MPQ\x1A", 4);
    out.close();
    
    auto placeholders = MPQValidator::getPlaceholderFiles(testFiles);
    
    EXPECT_EQ(placeholders.size(), 2);
    EXPECT_EQ(placeholders[0], testFiles[0]);
    EXPECT_EQ(placeholders[1], testFiles[1]);
    
    // Clean up
    for (const auto& file : testFiles) {
        std::remove(file.c_str());
    }
}

TEST(MPQValidatorTest, GetAuthenticFiles) {
    // Create test files
    std::vector<std::string> testFiles = {
        "test_placeholder1.mpq",
        "test_authentic1.mpq",
        "test_authentic2.mpq"
    };
    
    // Create placeholder file
    std::ofstream out1(testFiles[0], std::ios::binary);
    for (int i = 0; i < 100; i++) {
        out1 << 'X';
    }
    out1.close();
    
    // Create authentic files
    for (int i = 1; i < 3; i++) {
        std::ofstream out(testFiles[i], std::ios::binary);
        out.write("MPQ\x1A", 4);
        out.close();
    }
    
    auto authentic = MPQValidator::getAuthenticFiles(testFiles);
    
    EXPECT_EQ(authentic.size(), 2);
    EXPECT_EQ(authentic[0], testFiles[1]);
    EXPECT_EQ(authentic[1], testFiles[2]);
    
    // Clean up
    for (const auto& file : testFiles) {
        std::remove(file.c_str());
    }
}