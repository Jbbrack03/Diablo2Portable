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

TEST_F(ExtractionWizardUITest, CanGetExtractionOptions) {
    // Test that we can get extraction options from the wizard
    const auto& options = wizard->getExtractionOptions();
    
    // Should have default options
    EXPECT_EQ("", options.getOutputPath());
    EXPECT_TRUE(options.isAssetTypeEnabled(onboarding::AssetType::SPRITES));
    EXPECT_TRUE(options.isAssetTypeEnabled(onboarding::AssetType::AUDIO));
}

TEST_F(ExtractionWizardUITest, CanSetExtractionOptions) {
    // Test that we can modify extraction options through the wizard
    
    // Navigate to the extraction options step
    wizard->nextStep(); // FILE_SELECTION
    wizard->nextStep(); // EXTRACTION_OPTIONS
    EXPECT_EQ(wizard->getCurrentStep(), ExtractionWizardStep::EXTRACTION_OPTIONS);
    
    // Get a mutable reference to options
    auto& options = wizard->getExtractionOptions();
    
    // Set custom output path
    wizard->setOutputPath("/custom/output");
    EXPECT_EQ(options.getOutputPath(), "/custom/output");
    
    // Disable audio extraction
    wizard->setAssetTypeEnabled(onboarding::AssetType::AUDIO, false);
    EXPECT_FALSE(options.isAssetTypeEnabled(onboarding::AssetType::AUDIO));
    EXPECT_TRUE(options.isAssetTypeEnabled(onboarding::AssetType::SPRITES));
}

TEST_F(ExtractionWizardUITest, ExtractionOptionsStepInfo) {
    // Test that the extraction options step has proper information
    
    // Navigate to the extraction options step
    wizard->nextStep(); // FILE_SELECTION
    wizard->nextStep(); // EXTRACTION_OPTIONS
    
    auto stepInfo = wizard->getCurrentStepInfo();
    
    EXPECT_EQ(stepInfo.title, "Extraction Options");
    EXPECT_EQ(stepInfo.description, "Configure how your Diablo II assets will be extracted.");
    EXPECT_FALSE(stepInfo.instructions.empty());
    
    // Should have instructions for customizing the extraction
    EXPECT_TRUE(stepInfo.instructions.size() >= 2);
}

TEST_F(ExtractionWizardUITest, FileSelectionStepInfo) {
    // Test that the file selection step has proper information
    
    // Navigate to the file selection step
    wizard->nextStep(); // FILE_SELECTION
    
    auto stepInfo = wizard->getCurrentStepInfo();
    
    EXPECT_EQ(stepInfo.title, "Select Game Files");
    EXPECT_EQ(stepInfo.description, "Choose the location of your Diablo II game files for extraction.");
    EXPECT_FALSE(stepInfo.instructions.empty());
    
    // Should have instructions for file selection
    EXPECT_TRUE(stepInfo.instructions.size() >= 1);
}

TEST_F(ExtractionWizardUITest, ProgressStepInfo) {
    // Test that the progress step has proper information
    
    // Navigate to the progress step
    wizard->nextStep(); // FILE_SELECTION
    wizard->nextStep(); // EXTRACTION_OPTIONS
    wizard->nextStep(); // PROGRESS
    
    auto stepInfo = wizard->getCurrentStepInfo();
    
    EXPECT_EQ(stepInfo.title, "Extracting Assets");
    EXPECT_EQ(stepInfo.description, "Please wait while your Diablo II assets are being extracted and optimized.");
    EXPECT_FALSE(stepInfo.instructions.empty());
    
    // Should have instructions for progress monitoring
    EXPECT_TRUE(stepInfo.instructions.size() >= 1);
}

TEST_F(ExtractionWizardUITest, CompletionStepInfo) {
    // Test that the completion step has proper information
    
    // Navigate to the completion step
    wizard->nextStep(); // FILE_SELECTION
    wizard->nextStep(); // EXTRACTION_OPTIONS
    wizard->nextStep(); // PROGRESS
    wizard->nextStep(); // COMPLETION
    
    auto stepInfo = wizard->getCurrentStepInfo();
    
    EXPECT_EQ(stepInfo.title, "Extraction Complete");
    EXPECT_EQ(stepInfo.description, "Your Diablo II assets have been successfully extracted and are ready for use.");
    EXPECT_FALSE(stepInfo.instructions.empty());
    
    // Should have instructions for post-extraction actions
    EXPECT_TRUE(stepInfo.instructions.size() >= 3);
}

TEST_F(ExtractionWizardUITest, CanGetExtractionSummary) {
    // Test that we can get an extraction summary
    
    auto summary = wizard->getExtractionSummary();
    
    // Should have basic summary information
    EXPECT_GE(summary.totalFilesExtracted, 0);
    EXPECT_GE(summary.totalFilesProcessed, 0);
    EXPECT_GE(summary.timeTaken.count(), 0);
    EXPECT_GE(summary.storageUsed, 0);
    EXPECT_FALSE(summary.hasErrors()); // No errors by default
    EXPECT_TRUE(summary.successful); // Should be successful by default
}

} // namespace
} // namespace d2