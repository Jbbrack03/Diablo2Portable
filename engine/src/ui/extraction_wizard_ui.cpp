#include "ui/extraction_wizard_ui.h"

namespace d2 {

ExtractionWizardUI::ExtractionWizardUI() : currentStep(ExtractionWizardStep::WELCOME) {
}

ExtractionWizardUI::~ExtractionWizardUI() = default;

ExtractionWizardStep ExtractionWizardUI::getCurrentStep() const {
    return currentStep;
}

bool ExtractionWizardUI::nextStep() {
    if (currentStep == ExtractionWizardStep::WELCOME) {
        currentStep = ExtractionWizardStep::FILE_SELECTION;
        return true;
    }
    return false;
}

} // namespace d2