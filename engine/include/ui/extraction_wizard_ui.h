#pragma once

namespace d2 {

enum class ExtractionWizardStep {
    WELCOME,
    FILE_SELECTION,
    EXTRACTION_OPTIONS,
    PROGRESS,
    COMPLETION
};

class ExtractionWizardUI {
public:
    ExtractionWizardUI();
    ~ExtractionWizardUI();
    
    ExtractionWizardStep getCurrentStep() const;
    bool nextStep();

private:
    ExtractionWizardStep currentStep;
};

} // namespace d2