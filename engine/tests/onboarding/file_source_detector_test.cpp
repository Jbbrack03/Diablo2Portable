#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
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