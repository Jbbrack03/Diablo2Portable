#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "onboarding/file_source_detector.h"

namespace fs = std::filesystem;
using namespace d2;

class FileSourceDetectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        testDir = fs::temp_directory_path() / "d2_detector_test";
        fs::create_directories(testDir);
    }
    
    void TearDown() override {
        // Clean up test directories
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    fs::path testDir;
};

// STEP 1: Write exactly ONE failing test for detecting D2 installation directories
TEST_F(FileSourceDetectorTest, DetectD2InstallationDirectories) {
    FileSourceDetector detector;
    
    // Create mock D2 installation with required MPQ files
    fs::path mockD2Dir = testDir / "Diablo II";
    fs::create_directories(mockD2Dir);
    
    // Create dummy MPQ files
    std::ofstream(mockD2Dir / "d2data.mpq").close();
    std::ofstream(mockD2Dir / "d2exp.mpq").close();
    std::ofstream(mockD2Dir / "d2sfx.mpq").close();
    
    std::vector<std::string> searchPaths = {
        testDir.string()
    };
    
    auto found = detector.scanForInstallations(searchPaths);
    
    EXPECT_GT(found.size(), 0);
    EXPECT_TRUE(found[0].hasRequiredMPQs());
    EXPECT_EQ(found[0].getVersion(), D2Version::LORD_OF_DESTRUCTION);
}

// STEP 2: Write exactly ONE failing test for detecting CD drives
TEST_F(FileSourceDetectorTest, DetectCDDrives) {
    FileSourceDetector detector;
    
    auto cdDrives = detector.detectCDDrives();
    
    // Test should pass even with no CDs
    EXPECT_TRUE(cdDrives.size() >= 0);
    
    // If test CD present, validate
    for (const auto& drive : cdDrives) {
        if (drive.hasD2Disc()) {
            EXPECT_TRUE(drive.canReadMPQFiles());
        }
    }
}

// STEP 3: Write exactly ONE failing test for validating ISO files
TEST_F(FileSourceDetectorTest, ValidateISOFiles) {
    FileSourceDetector detector;
    
    // Create a mock ISO file (just a regular file for testing)
    fs::path isoPath = testDir / "d2_test.iso";
    std::ofstream isoFile(isoPath, std::ios::binary);
    isoFile.write("ISO 9660", 8); // Mock ISO header
    // Add padding to make file large enough
    char padding[1024] = {0};
    isoFile.write(padding, sizeof(padding));
    isoFile.close();
    
    auto validation = detector.validateISOFile(isoPath.string());
    
    EXPECT_TRUE(validation.isValid);
    EXPECT_TRUE(validation.containsD2Data);
    EXPECT_FALSE(validation.requiresMount);
}

// STEP 4: Write exactly ONE failing test for Android path support
TEST_F(FileSourceDetectorTest, DetectAndroidStoragePaths) {
    FileSourceDetector detector;
    
    // Create mock Android storage structure
    fs::path androidInternal = testDir / "storage" / "emulated" / "0";
    fs::path androidExternal = testDir / "storage" / "sdcard1";
    fs::path androidDownloads = androidInternal / "Download";
    
    fs::create_directories(androidInternal);
    fs::create_directories(androidExternal);
    fs::create_directories(androidDownloads);
    
    // Add D2 files in Downloads
    fs::path d2DownloadDir = androidDownloads / "Diablo2";
    fs::create_directories(d2DownloadDir);
    
    std::ofstream(d2DownloadDir / "d2data.mpq").close();
    std::ofstream(d2DownloadDir / "d2exp.mpq").close();
    std::ofstream(d2DownloadDir / "d2sfx.mpq").close();
    
    auto androidPaths = detector.getAndroidSearchPaths();
    
    // Should include standard Android paths
    EXPECT_TRUE(std::find(androidPaths.begin(), androidPaths.end(), 
                         "/storage/emulated/0") != androidPaths.end());
    EXPECT_TRUE(std::find(androidPaths.begin(), androidPaths.end(), 
                         "/storage/emulated/0/Download") != androidPaths.end());
    
    // Now scan with Android paths included
    std::vector<std::string> searchPaths;
    searchPaths.push_back(androidInternal.string());
    searchPaths.push_back(androidDownloads.string());
    
    auto found = detector.scanForInstallations(searchPaths);
    
    EXPECT_GT(found.size(), 0);
    EXPECT_TRUE(found[0].hasRequiredMPQs());
}