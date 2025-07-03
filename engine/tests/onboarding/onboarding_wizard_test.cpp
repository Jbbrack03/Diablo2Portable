#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
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