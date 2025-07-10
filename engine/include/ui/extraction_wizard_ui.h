#pragma once

#include <string>
#include <vector>

namespace d2 {

enum class ExtractionWizardStep {
    WELCOME,
    FILE_SELECTION,
    EXTRACTION_OPTIONS,
    PROGRESS,
    COMPLETION
};

struct StepInfo {
    std::string title;
    std::string description;
    std::vector<std::string> instructions;
};

class ExtractionWizardUI {
public:
    ExtractionWizardUI();
    ~ExtractionWizardUI();
    
    ExtractionWizardStep getCurrentStep() const;
    StepInfo getCurrentStepInfo() const;
    bool nextStep();
    float getOverallProgress() const;

private:
    ExtractionWizardStep currentStep;
};

} // namespace d2