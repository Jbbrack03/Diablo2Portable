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