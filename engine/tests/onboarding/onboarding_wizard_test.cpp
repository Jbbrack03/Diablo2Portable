#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "onboarding/onboarding_wizard.h"

namespace fs = std::filesystem;
using namespace d2;

class OnboardingWizardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        testDir = fs::temp_directory_path() / "d2_wizard_test";
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

// STEP 1: Write exactly ONE failing test for file browser support
TEST_F(OnboardingWizardTest, ShowFileBrowser) {
    OnboardingWizard wizard;
    
    EXPECT_TRUE(wizard.showFileBrowser());
    EXPECT_TRUE(wizard.canSelectMPQFiles());
}

// STEP 2: Write exactly ONE failing test for MPQ validation and import
TEST_F(OnboardingWizardTest, ValidateAndImportMPQFiles) {
    OnboardingWizard wizard;
    
    // Create mock MPQ files
    fs::path mpqDir = testDir / "mpqs";
    fs::create_directories(mpqDir);
    
    std::ofstream(mpqDir / "d2data.mpq").close();
    std::ofstream(mpqDir / "d2exp.mpq").close();
    
    std::vector<std::string> files = {
        (mpqDir / "d2data.mpq").string(),
        (mpqDir / "d2exp.mpq").string()
    };
    
    EXPECT_TRUE(wizard.validateMPQFiles(files));
    EXPECT_TRUE(wizard.importFiles(files));
}

// STEP 3: Write exactly ONE failing test for progress tracking
TEST_F(OnboardingWizardTest, TrackImportProgress) {
    OnboardingWizard wizard;
    
    // Create larger mock MPQ files for realistic progress tracking
    fs::path mpqDir = testDir / "progress_test";
    fs::create_directories(mpqDir);
    
    // Create files with some content
    for (const auto& filename : {"d2data.mpq", "d2exp.mpq", "d2sfx.mpq"}) {
        std::ofstream file(mpqDir / filename, std::ios::binary);
        // Write 1MB of data to simulate real MPQ files
        std::vector<char> data(1024 * 1024, 'X');
        file.write(data.data(), data.size());
    }
    
    std::vector<std::string> files = {
        (mpqDir / "d2data.mpq").string(),
        (mpqDir / "d2exp.mpq").string(),
        (mpqDir / "d2sfx.mpq").string()
    };
    
    bool progressCalled = false;
    float lastProgress = 0.0f;
    
    wizard.setProgressCallback([&progressCalled, &lastProgress](float progress) {
        progressCalled = true;
        lastProgress = progress;
        // Progress should be between 0 and 1
        EXPECT_GE(progress, 0.0f);
        EXPECT_LE(progress, 1.0f);
    });
    
    EXPECT_TRUE(wizard.importWithProgress(files));
    EXPECT_TRUE(progressCalled);
    EXPECT_FLOAT_EQ(lastProgress, 1.0f); // Should reach 100% when done
}

// STEP 4: Write exactly ONE failing test for error handling and recovery
TEST_F(OnboardingWizardTest, HandleMissingFiles) {
    OnboardingWizard wizard;
    
    // Set import directory to a test location instead of vendor/mpq
    fs::path testImportDir = testDir / "test_import";
    fs::create_directories(testImportDir);
    wizard.setImportDirectory(testImportDir.string());
    
    auto result = wizard.checkRequiredFiles();
    
    // Since no files have been imported yet, some should be missing
    EXPECT_FALSE(result.allFilesPresent);
    EXPECT_GT(result.missingFiles.size(), 0);
    
    // Should include at least the core MPQ files
    auto& missing = result.missingFiles;
    EXPECT_TRUE(std::find(missing.begin(), missing.end(), "d2data.mpq") != missing.end());
    EXPECT_TRUE(std::find(missing.begin(), missing.end(), "d2exp.mpq") != missing.end());
    EXPECT_TRUE(std::find(missing.begin(), missing.end(), "d2sfx.mpq") != missing.end());
}

// STEP 5: Write exactly ONE failing test for error recovery
TEST_F(OnboardingWizardTest, RecoverFromPartialImport) {
    OnboardingWizard wizard;
    
    // Set import directory to test location
    fs::path testImportDir = testDir / "recover_import";
    fs::create_directories(testImportDir);
    wizard.setImportDirectory(testImportDir.string());
    
    // Create only some of the required files
    fs::path mpqDir = testDir / "partial_import";
    fs::create_directories(mpqDir);
    
    std::ofstream(mpqDir / "d2data.mpq").close();
    // Missing d2exp.mpq and d2sfx.mpq
    
    std::vector<std::string> files = {
        (mpqDir / "d2data.mpq").string()
    };
    
    // First import should succeed but be incomplete
    EXPECT_TRUE(wizard.importFiles(files));
    
    auto result = wizard.checkRequiredFiles();
    EXPECT_FALSE(result.allFilesPresent);
    EXPECT_GT(result.missingFiles.size(), 0);
    
    // Now add the missing files
    std::ofstream(mpqDir / "d2exp.mpq").close();
    std::ofstream(mpqDir / "d2sfx.mpq").close();
    
    std::vector<std::string> additionalFiles = {
        (mpqDir / "d2exp.mpq").string(),
        (mpqDir / "d2sfx.mpq").string()
    };
    
    // Import the missing files
    EXPECT_TRUE(wizard.importFiles(additionalFiles));
    
    // Now check again - we should still be missing some files (music, speech, video)
    auto finalResult = wizard.checkRequiredFiles();
    EXPECT_FALSE(finalResult.allFilesPresent); // Still missing some files
    EXPECT_GT(finalResult.missingFiles.size(), 0);
}