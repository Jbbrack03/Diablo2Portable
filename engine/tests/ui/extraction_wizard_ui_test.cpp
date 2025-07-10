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

} // namespace
} // namespace d2