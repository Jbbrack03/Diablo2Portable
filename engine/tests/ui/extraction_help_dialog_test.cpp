#include <gtest/gtest.h>
#include "ui/extraction_help_dialog.h"

namespace d2 {

class ExtractionHelpDialogTest : public ::testing::Test {
protected:
    void SetUp() override {
        helpDialog = std::make_unique<ExtractionHelpDialog>();
    }

    std::unique_ptr<ExtractionHelpDialog> helpDialog;
};

// Test 1: Create extraction help dialog
TEST_F(ExtractionHelpDialogTest, CreateExtractionHelpDialog) {
    EXPECT_NE(helpDialog, nullptr);
    EXPECT_FALSE(helpDialog->isVisible());
}

// Test 2: Show and hide help dialog
TEST_F(ExtractionHelpDialogTest, ShowAndHideHelpDialog) {
    // Initially not visible
    EXPECT_FALSE(helpDialog->isVisible());
    
    // Show the dialog
    helpDialog->show();
    EXPECT_TRUE(helpDialog->isVisible());
    
    // Hide the dialog
    helpDialog->hide();
    EXPECT_FALSE(helpDialog->isVisible());
}

} // namespace d2