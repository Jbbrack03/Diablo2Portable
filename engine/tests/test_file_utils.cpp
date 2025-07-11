#include "utils/file_utils.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

class FileUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for testing
        testDir = std::filesystem::temp_directory_path() / "fileutils_test";
        std::filesystem::create_directories(testDir);
        
        // Create test files
        testFile = testDir / "test.txt";
        binaryFile = testDir / "test.bin";
        nonExistentFile = testDir / "nonexistent.txt";
        
        // Create test file content
        testContent = "Hello, World! This is a test file.";
        binaryContent = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64}; // "Hello World" in hex
        
        // Write test files
        {
            std::ofstream file(testFile);
            file << testContent;
        }
        
        {
            std::ofstream file(binaryFile, std::ios::binary);
            file.write(reinterpret_cast<const char*>(binaryContent.data()), binaryContent.size());
        }
    }
    
    void TearDown() override {
        // Clean up test directory
        std::filesystem::remove_all(testDir);
    }
    
    std::filesystem::path testDir;
    std::filesystem::path testFile;
    std::filesystem::path binaryFile;
    std::filesystem::path nonExistentFile;
    std::string testContent;
    std::vector<uint8_t> binaryContent;
};

TEST_F(FileUtilsTest, ValidateFileExists) {
    EXPECT_TRUE(d2::utils::FileUtils::validateFileExists(testFile.string()));
    EXPECT_FALSE(d2::utils::FileUtils::validateFileExists(nonExistentFile.string()));
}

TEST_F(FileUtilsTest, SafeOpenBinaryFileForReading) {
    std::ifstream file;
    EXPECT_TRUE(d2::utils::FileUtils::safeOpenBinaryFileForReading(binaryFile.string(), file));
    EXPECT_TRUE(file.is_open());
    file.close();
    
    EXPECT_FALSE(d2::utils::FileUtils::safeOpenBinaryFileForReading(nonExistentFile.string(), file));
    EXPECT_FALSE(file.is_open());
}

TEST_F(FileUtilsTest, SafeOpenTextFileForReading) {
    std::ifstream file;
    EXPECT_TRUE(d2::utils::FileUtils::safeOpenTextFileForReading(testFile.string(), file));
    EXPECT_TRUE(file.is_open());
    file.close();
    
    EXPECT_FALSE(d2::utils::FileUtils::safeOpenTextFileForReading(nonExistentFile.string(), file));
    EXPECT_FALSE(file.is_open());
}

TEST_F(FileUtilsTest, SafeOpenBinaryFileForWriting) {
    std::filesystem::path newFile = testDir / "new_binary.bin";
    std::ofstream file;
    EXPECT_TRUE(d2::utils::FileUtils::safeOpenBinaryFileForWriting(newFile.string(), file));
    EXPECT_TRUE(file.is_open());
    file.close();
    
    EXPECT_TRUE(std::filesystem::exists(newFile));
}

TEST_F(FileUtilsTest, SafeOpenTextFileForWriting) {
    std::filesystem::path newFile = testDir / "new_text.txt";
    std::ofstream file;
    EXPECT_TRUE(d2::utils::FileUtils::safeOpenTextFileForWriting(newFile.string(), file));
    EXPECT_TRUE(file.is_open());
    file.close();
    
    EXPECT_TRUE(std::filesystem::exists(newFile));
}

TEST_F(FileUtilsTest, ValidateFileHeader) {
    std::vector<uint8_t> expectedHeader = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    EXPECT_TRUE(d2::utils::FileUtils::validateFileHeader(binaryFile.string(), expectedHeader));
    
    std::vector<uint8_t> wrongHeader = {0x57, 0x6f, 0x72, 0x6c, 0x64}; // "World"
    EXPECT_FALSE(d2::utils::FileUtils::validateFileHeader(binaryFile.string(), wrongHeader));
}

TEST_F(FileUtilsTest, ValidateFileHeaderString) {
    EXPECT_TRUE(d2::utils::FileUtils::validateFileHeader(binaryFile.string(), "Hello"));
    EXPECT_FALSE(d2::utils::FileUtils::validateFileHeader(binaryFile.string(), "World"));
}

TEST_F(FileUtilsTest, ReadEntireFileBinary) {
    std::vector<uint8_t> data;
    EXPECT_TRUE(d2::utils::FileUtils::readEntireFile(binaryFile.string(), data));
    EXPECT_EQ(data, binaryContent);
}

TEST_F(FileUtilsTest, ReadEntireFileText) {
    std::string data;
    EXPECT_TRUE(d2::utils::FileUtils::readEntireFile(testFile.string(), data));
    EXPECT_EQ(data, testContent);
}

TEST_F(FileUtilsTest, WriteEntireFileBinary) {
    std::filesystem::path newFile = testDir / "write_test.bin";
    std::vector<uint8_t> testData = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    EXPECT_TRUE(d2::utils::FileUtils::writeEntireFile(newFile.string(), testData));
    EXPECT_TRUE(std::filesystem::exists(newFile));
    
    // Verify content
    std::vector<uint8_t> readData;
    EXPECT_TRUE(d2::utils::FileUtils::readEntireFile(newFile.string(), readData));
    EXPECT_EQ(readData, testData);
}

TEST_F(FileUtilsTest, WriteEntireFileText) {
    std::filesystem::path newFile = testDir / "write_test.txt";
    std::string testData = "This is a test string.";
    
    EXPECT_TRUE(d2::utils::FileUtils::writeEntireFile(newFile.string(), testData));
    EXPECT_TRUE(std::filesystem::exists(newFile));
    
    // Verify content
    std::string readData;
    EXPECT_TRUE(d2::utils::FileUtils::readEntireFile(newFile.string(), readData));
    EXPECT_EQ(readData, testData);
}

TEST_F(FileUtilsTest, GetFileSize) {
    EXPECT_EQ(d2::utils::FileUtils::getFileSize(testFile.string()), testContent.size());
    EXPECT_EQ(d2::utils::FileUtils::getFileSize(binaryFile.string()), binaryContent.size());
    EXPECT_EQ(d2::utils::FileUtils::getFileSize(nonExistentFile.string()), 0);
}

TEST_F(FileUtilsTest, IsFileEmpty) {
    // Create an empty file
    std::filesystem::path emptyFile = testDir / "empty.txt";
    std::ofstream file(emptyFile);
    file.close();
    
    EXPECT_TRUE(d2::utils::FileUtils::isFileEmpty(emptyFile.string()));
    EXPECT_FALSE(d2::utils::FileUtils::isFileEmpty(testFile.string()));
    EXPECT_TRUE(d2::utils::FileUtils::isFileEmpty(nonExistentFile.string()));
}

TEST_F(FileUtilsTest, CreateParentDirectories) {
    std::filesystem::path newPath = testDir / "subdir" / "subsubdir" / "file.txt";
    EXPECT_TRUE(d2::utils::FileUtils::createParentDirectories(newPath.string()));
    EXPECT_TRUE(std::filesystem::exists(newPath.parent_path()));
}

TEST_F(FileUtilsTest, SafeCloseFile) {
    std::ifstream file;
    EXPECT_TRUE(d2::utils::FileUtils::safeOpenBinaryFileForReading(testFile.string(), file));
    EXPECT_TRUE(d2::utils::FileUtils::safeCloseFile(file, testFile.string()));
    EXPECT_FALSE(file.is_open());
}

TEST_F(FileUtilsTest, ErrorHandling) {
    // Test error messages
    std::ifstream file;
    EXPECT_FALSE(d2::utils::FileUtils::safeOpenBinaryFileForReading(nonExistentFile.string(), file));
    std::string error = d2::utils::FileUtils::getLastError();
    EXPECT_FALSE(error.empty());
    EXPECT_NE(error.find("does not exist"), std::string::npos);
}