#include "ui/extraction_wizard_ui.h"

namespace d2 {

ExtractionWizardUI::ExtractionWizardUI() : currentStep(ExtractionWizardStep::WELCOME) {
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

} // namespace d2