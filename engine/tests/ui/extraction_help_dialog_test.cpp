#include <gtest/gtest.h>
#include "ui/extraction_help_dialog.h"
#include "ui/help_system.h"

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

// Test 3: Set context for help dialog
TEST_F(ExtractionHelpDialogTest, SetHelpContext) {
    // Set context to file selection
    helpDialog->setContext(ExtractionHelpContext::FILE_SELECTION);
    EXPECT_EQ(helpDialog->getCurrentContext(), ExtractionHelpContext::FILE_SELECTION);
    
    // Set context to extraction options
    helpDialog->setContext(ExtractionHelpContext::EXTRACTION_OPTIONS);
    EXPECT_EQ(helpDialog->getCurrentContext(), ExtractionHelpContext::EXTRACTION_OPTIONS);
    
    // Set context to progress
    helpDialog->setContext(ExtractionHelpContext::PROGRESS);
    EXPECT_EQ(helpDialog->getCurrentContext(), ExtractionHelpContext::PROGRESS);
}

// Test 4: Get context-specific help content
TEST_F(ExtractionHelpDialogTest, GetContextSpecificHelp) {
    // File selection context
    helpDialog->setContext(ExtractionHelpContext::FILE_SELECTION);
    auto help = helpDialog->getHelpContent();
    EXPECT_FALSE(help.title.empty());
    EXPECT_FALSE(help.content.empty());
    EXPECT_TRUE(help.title.find("File Selection") != std::string::npos);
    
    // Extraction options context
    helpDialog->setContext(ExtractionHelpContext::EXTRACTION_OPTIONS);
    help = helpDialog->getHelpContent();
    EXPECT_FALSE(help.title.empty());
    EXPECT_FALSE(help.content.empty());
    EXPECT_TRUE(help.title.find("Extraction Options") != std::string::npos);
    
    // Progress context
    helpDialog->setContext(ExtractionHelpContext::PROGRESS);
    help = helpDialog->getHelpContent();
    EXPECT_FALSE(help.title.empty());
    EXPECT_FALSE(help.content.empty());
    EXPECT_TRUE(help.title.find("Extraction Progress") != std::string::npos);
}

// Test 5: Get troubleshooting tips
TEST_F(ExtractionHelpDialogTest, GetTroubleshootingTips) {
    // Get troubleshooting tips for file selection issues
    helpDialog->setContext(ExtractionHelpContext::FILE_SELECTION);
    auto tips = helpDialog->getTroubleshootingTips();
    EXPECT_FALSE(tips.empty());
    EXPECT_GE(tips.size(), 2); // At least 2 tips
    
    // Verify we have tips about common file selection issues
    bool foundPermissionTip = false;
    bool foundLocationTip = false;
    for (const auto& tip : tips) {
        if (tip.find("permission") != std::string::npos || 
            tip.find("Permission") != std::string::npos) {
            foundPermissionTip = true;
        }
        if (tip.find("location") != std::string::npos || 
            tip.find("Location") != std::string::npos) {
            foundLocationTip = true;
        }
    }
    EXPECT_TRUE(foundPermissionTip);
    EXPECT_TRUE(foundLocationTip);
}

// Test 6: Integration with HelpSystem
TEST_F(ExtractionHelpDialogTest, IntegrationWithHelpSystem) {
    // Set help system
    auto helpSystem = std::make_shared<HelpSystem>();
    helpDialog->setHelpSystem(helpSystem);
    
    // Verify help dialog can access help system topics
    helpDialog->setContext(ExtractionHelpContext::FILE_SELECTION);
    auto relatedTopics = helpDialog->getRelatedHelpTopics();
    EXPECT_FALSE(relatedTopics.empty());
    
    // Should find asset extraction topic
    bool foundAssetExtraction = false;
    for (const auto& topicId : relatedTopics) {
        if (topicId == "asset-extraction") {
            foundAssetExtraction = true;
            break;
        }
    }
    EXPECT_TRUE(foundAssetExtraction);
}

} // namespace d2