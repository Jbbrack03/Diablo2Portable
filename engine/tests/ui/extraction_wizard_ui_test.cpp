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

} // namespace
} // namespace d2