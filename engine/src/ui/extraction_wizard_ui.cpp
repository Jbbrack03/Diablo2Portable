#include "ui/extraction_wizard_ui.h"

namespace d2 {

ExtractionWizardUI::ExtractionWizardUI() : currentStep(ExtractionWizardStep::WELCOME) {
    // Initialize extraction summary with default values
    extractionSummary.totalFilesExtracted = 0;
    extractionSummary.totalFilesProcessed = 0;
    extractionSummary.timeTaken = std::chrono::seconds(0);
    extractionSummary.storageUsed = 0;
    extractionSummary.successful = true;
}

ExtractionWizardUI::~ExtractionWizardUI() = default;

ExtractionWizardStep ExtractionWizardUI::getCurrentStep() const {
    return currentStep;
}

StepInfo ExtractionWizardUI::getCurrentStepInfo() const {
    StepInfo info;
    switch (currentStep) {
        case ExtractionWizardStep::WELCOME:
            info.title = "Welcome to Diablo II Extraction";
            info.description = "This wizard will guide you through extracting your Diablo II game files for use on mobile devices.";
            info.instructions = {"Click Next to begin the extraction process."};
            break;
        case ExtractionWizardStep::FILE_SELECTION:
            info.title = "Select Game Files";
            info.description = "Choose the location of your Diablo II game files for extraction.";
            info.instructions = {"Browse to your Diablo II installation directory or select MPQ files directly."};
            break;
        case ExtractionWizardStep::EXTRACTION_OPTIONS:
            info.title = "Extraction Options";
            info.description = "Configure how your Diablo II assets will be extracted.";
            info.instructions = {
                "Choose a custom output directory or use the default location.",
                "Select which asset types you want to extract (sprites, audio, data tables, videos)."
            };
            break;
        case ExtractionWizardStep::PROGRESS:
            info.title = "Extracting Assets";
            info.description = "Please wait while your Diablo II assets are being extracted and optimized.";
            info.instructions = {"The extraction process may take several minutes depending on your system performance."};
            break;
        case ExtractionWizardStep::COMPLETION:
            info.title = "Extraction Complete";
            info.description = "Your Diablo II assets have been successfully extracted and are ready for use.";
            info.instructions = {
                "Click 'Browse Assets' to explore your extracted files.",
                "Click 'Verify Integrity' to validate all files were extracted correctly.",
                "Click 'Finish' to complete the wizard and start playing."
            };
            break;
        default:
            info.title = "Unknown Step";
            info.description = "Step information not available.";
            break;
    }
    return info;
}

bool ExtractionWizardUI::nextStep() {
    switch (currentStep) {
        case ExtractionWizardStep::WELCOME:
            currentStep = ExtractionWizardStep::FILE_SELECTION;
            return true;
        case ExtractionWizardStep::FILE_SELECTION:
            currentStep = ExtractionWizardStep::EXTRACTION_OPTIONS;
            return true;
        case ExtractionWizardStep::EXTRACTION_OPTIONS:
            currentStep = ExtractionWizardStep::PROGRESS;
            return true;
        case ExtractionWizardStep::PROGRESS:
            currentStep = ExtractionWizardStep::COMPLETION;
            return true;
        case ExtractionWizardStep::COMPLETION:
            return false; // Can't go beyond completion
        default:
            return false;
    }
}

float ExtractionWizardUI::getOverallProgress() const {
    switch (currentStep) {
        case ExtractionWizardStep::WELCOME:
            return 0.0f;
        case ExtractionWizardStep::FILE_SELECTION:
            return 0.25f;
        case ExtractionWizardStep::EXTRACTION_OPTIONS:
            return 0.5f;
        case ExtractionWizardStep::PROGRESS:
            return 0.75f;
        case ExtractionWizardStep::COMPLETION:
            return 1.0f;
        default:
            return 0.0f;
    }
}

ExtractionSummary ExtractionWizardUI::getExtractionSummary() const {
    return extractionSummary;
}

void ExtractionWizardUI::updateExtractionSummary(int filesExtracted, int filesProcessed, 
                                                std::chrono::seconds timeTaken, size_t storageUsed) {
    extractionSummary.totalFilesExtracted = filesExtracted;
    extractionSummary.totalFilesProcessed = filesProcessed;
    extractionSummary.timeTaken = timeTaken;
    extractionSummary.storageUsed = storageUsed;
    extractionSummary.successful = true;
}

bool ExtractionWizardUI::launchAssetBrowser(const std::string& assetPath) {
    // Simple implementation - just verify the path is not empty
    if (assetPath.empty()) {
        return false;
    }
    
    // For now, just return true indicating successful launch
    // In a real implementation, this would initialize and show the asset browser
    return true;
}

VerificationResult ExtractionWizardUI::verifyExtractedAssets(const std::string& assetPath) {
    // Simple implementation - create a basic verification result
    VerificationResult result;
    
    if (assetPath.empty()) {
        result.isComplete = false;
        result.validatedFiles = 0;
        return result;
    }
    
    // For now, return a successful verification
    result.isComplete = true;
    result.validatedFiles = 0;
    return result;
}

} // namespace d2