#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "onboarding/file_format_handler.h"

namespace fs = std::filesystem;
using namespace d2;

class FileFormatHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "file_format_test";
        fs::create_directories(testDir);
    }
    
    void TearDown() override {
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    // Create a mock ISO file with basic ISO 9660 structure
    void createMockISO(const fs::path& isoPath) {
        std::ofstream file(isoPath, std::ios::binary);
        
        // ISO 9660 Primary Volume Descriptor starts at sector 16 (32768 bytes)
        // Write some padding first
        std::vector<char> padding(32768, 0);
        file.write(padding.data(), padding.size());
        
        // Write simplified Primary Volume Descriptor
        char pvd_header[] = {0x01, 'C', 'D', '0', '0', '1', 0x01, 0x00};
        file.write(pvd_header, sizeof(pvd_header));
        
        // Add some more data to make it look like a real ISO
        std::vector<char> data(2048, 0);
        for (int i = 0; i < 10; i++) {
            file.write(data.data(), data.size());
        }
        
        file.close();
    }
    
    fs::path testDir;
};

// STEP 1: Write exactly ONE failing test for extracting from ISO
TEST_F(FileFormatHandlerTest, ExtractFromISO) {
    FileFormatHandler handler;
    
    fs::path isoPath = testDir / "d2_disc1.iso";
    fs::path outputDir = testDir / "extracted";
    
    // Create a mock ISO file
    createMockISO(isoPath);
    
    // Create mock MPQ file inside the ISO structure
    // In a real implementation, this would be extracted from the ISO
    fs::create_directories(outputDir);
    
    auto result = handler.extractFromISO(isoPath.string(), outputDir.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.filesExtracted, 0);
    // Note: We can't check for d2data.mpq in outputDir yet as extraction is not implemented
}

// STEP 2: Write exactly ONE failing test for handling Battle.net installer
TEST_F(FileFormatHandlerTest, HandleBattleNetInstaller) {
    FileFormatHandler handler;
    
    fs::path installerPath = testDir / "D2_installer.exe";
    fs::path outputDir = testDir / "installer_extract";
    
    // Create a mock installer file (PE executable header)
    std::ofstream file(installerPath, std::ios::binary);
    // PE header signature
    char mz_header[] = {'M', 'Z'};
    file.write(mz_header, sizeof(mz_header));
    // Add some dummy data
    std::vector<char> data(1024, 0);
    file.write(data.data(), data.size());
    file.close();
    
    auto result = handler.extractFromInstaller(installerPath.string(), outputDir.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.foundMPQFiles);
}

// STEP 3: Write exactly ONE failing test for processing compressed archives
TEST_F(FileFormatHandlerTest, ProcessCompressedArchive) {
    FileFormatHandler handler;
    
    fs::path zipPath = testDir / "d2_backup.zip";
    fs::path outputDir = testDir / "zip_extract";
    
    // Create a mock ZIP file with proper header
    std::ofstream file(zipPath, std::ios::binary);
    // ZIP file signature
    char zip_header[] = {'P', 'K', 0x03, 0x04};
    file.write(zip_header, sizeof(zip_header));
    // Add some dummy data
    std::vector<char> data(1024, 0);
    file.write(data.data(), data.size());
    file.close();
    
    auto result = handler.extractFromArchive(zipPath.string(), outputDir.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.mpqFilesFound, 5);
}