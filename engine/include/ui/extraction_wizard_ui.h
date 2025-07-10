#pragma once

#include <string>
#include <vector>
#include "onboarding/extraction_options.h"

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
    
    // Get the current extraction options
    const onboarding::ExtractionOptions& getExtractionOptions() const { return extractionOptions; }

private:
    ExtractionWizardStep currentStep;
    onboarding::ExtractionOptions extractionOptions;
};

} // namespace d2