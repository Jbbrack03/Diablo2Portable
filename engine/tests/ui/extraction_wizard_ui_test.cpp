#include <gtest/gtest.h>
#include "ui/extraction_wizard_ui.h"

namespace d2 {
namespace {

class ExtractionWizardUITest : public ::testing::Test {
protected:
    void SetUp() override {
        wizard = std::make_unique<ExtractionWizardUI>();
    }
    
    std::unique_ptr<ExtractionWizardUI> wizard;
};

TEST_F(ExtractionWizardUITest, CanCreateExtractionWizardUI) {
    // Test that we can create an ExtractionWizardUI instance
    EXPECT_TRUE(wizard != nullptr);
    EXPECT_EQ(wizard->getCurrentStep(), ExtractionWizardStep::WELCOME);
}

TEST_F(ExtractionWizardUITest, CanNavigateToNextStep) {
    // Test that we can navigate to the next step in the wizard
    EXPECT_EQ(wizard->getCurrentStep(), ExtractionWizardStep::WELCOME);
    
    bool success = wizard->nextStep();
    EXPECT_TRUE(success);
    EXPECT_EQ(wizard->getCurrentStep(), ExtractionWizardStep::FILE_SELECTION);
}

TEST_F(ExtractionWizardUITest, CanGetStepInformation) {
    // Test that we can get information about the current step
    auto stepInfo = wizard->getCurrentStepInfo();
    
    EXPECT_EQ(stepInfo.title, "Welcome to Diablo II Extraction");
    EXPECT_EQ(stepInfo.description, "This wizard will guide you through extracting your Diablo II game files for use on mobile devices.");
    EXPECT_FALSE(stepInfo.instructions.empty());
}

TEST_F(ExtractionWizardUITest, CanTrackProgress) {
    // Test that we can track overall progress through the wizard
    float progress = wizard->getOverallProgress();
    EXPECT_EQ(progress, 0.0f); // At the beginning (WELCOME step)
    
    wizard->nextStep(); // Move to FILE_SELECTION
    progress = wizard->getOverallProgress();
    EXPECT_EQ(progress, 0.25f); // 1 of 4 steps completed (WELCOME done)
}

} // namespace
} // namespace d2