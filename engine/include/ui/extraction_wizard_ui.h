#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "onboarding/extraction_options.h"
#include "tools/asset_browser_backend.h"

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

struct ExtractionSummary {
    int totalFilesExtracted = 0;
    int totalFilesProcessed = 0;
    std::chrono::seconds timeTaken{0};
    size_t storageUsed = 0;
    std::vector<std::string> errorMessages;
    bool successful = false;
    
    bool hasErrors() const {
        return !errorMessages.empty();
    }
};

class ExtractionWizardUI {
public:
    ExtractionWizardUI();
    ~ExtractionWizardUI();
    
    ExtractionWizardStep getCurrentStep() const;
    StepInfo getCurrentStepInfo() const;
    bool nextStep();
    float getOverallProgress() const;
    
    // Extraction summary functionality
    ExtractionSummary getExtractionSummary() const;
    
    // Asset browser integration
    bool launchAssetBrowser(const std::string& assetPath);
    
    // Get the current extraction options
    const onboarding::ExtractionOptions& getExtractionOptions() const { return extractionOptions; }
    onboarding::ExtractionOptions& getExtractionOptions() { return extractionOptions; }
    
    // Convenience methods for setting extraction options
    void setOutputPath(const std::string& path) { extractionOptions.setOutputPath(path); }
    void setAssetTypeEnabled(onboarding::AssetType type, bool enabled) { 
        extractionOptions.setAssetTypeEnabled(type, enabled); 
    }

private:
    ExtractionWizardStep currentStep;
    onboarding::ExtractionOptions extractionOptions;
};

} // namespace d2