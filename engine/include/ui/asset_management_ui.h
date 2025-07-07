#pragma once

#include <string>
#include <vector>

namespace d2 {
namespace ui {

struct ExtractionProgress {
    std::string currentFile;
    float percentage = 0.0f;
    int filesExtracted = 0;
    int totalFiles = 0;
};

struct ValidationStatus {
    bool isValidating = false;
    std::string assetPath;
    int totalAssets = 0;
    int missingAssets = 0;
    std::vector<std::string> missingFiles;
};

class AssetManagementUI {
public:
    AssetManagementUI() : initialized_(true) {}
    ~AssetManagementUI() = default;
    
    bool isInitialized() const { return initialized_; }
    
    void setExtractionProgress(const std::string& filename, float percentage, 
                              int filesExtracted, int totalFiles) {
        progress_.currentFile = filename;
        progress_.percentage = percentage;
        progress_.filesExtracted = filesExtracted;
        progress_.totalFiles = totalFiles;
    }
    
    ExtractionProgress getExtractionProgress(const std::string& filename) const {
        return progress_;
    }
    
    void startAssetValidation(const std::string& assetPath) {
        validationStatus_.isValidating = true;
        validationStatus_.assetPath = assetPath;
        validationStatus_.totalAssets = 0;
        validationStatus_.missingAssets = 0;
        validationStatus_.missingFiles.clear();
    }
    
    void setValidationResult(int totalAssets, int missingAssets, 
                            const std::vector<std::string>& missingFiles) {
        validationStatus_.isValidating = false;
        validationStatus_.totalAssets = totalAssets;
        validationStatus_.missingAssets = missingAssets;
        validationStatus_.missingFiles = missingFiles;
    }
    
    ValidationStatus getValidationStatus() const {
        return validationStatus_;
    }
    
private:
    bool initialized_ = false;
    ExtractionProgress progress_;
    ValidationStatus validationStatus_;
};

} // namespace ui
} // namespace d2