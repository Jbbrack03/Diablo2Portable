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
    if (currentStep == ExtractionWizardStep::WELCOME) {
        currentStep = ExtractionWizardStep::FILE_SELECTION;
        return true;
    }
    return false;
}

} // namespace d2