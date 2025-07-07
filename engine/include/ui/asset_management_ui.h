#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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

struct StorageInfo {
    size_t totalSpace = 0;
    size_t usedSpace = 0;
    size_t availableSpace = 0;
    float usagePercentage = 0.0f;
};

class AssetManagementUI {
public:
    AssetManagementUI() : initialized_(true) {}
    ~AssetManagementUI() = default;
    
    bool isInitialized() const { return initialized_; }
    
    void setExtractionProgress(const std::string& filename, float percentage, 
                              int filesExtracted, int totalFiles) {
        ExtractionProgress prog;
        prog.currentFile = filename;
        prog.percentage = percentage;
        prog.filesExtracted = filesExtracted;
        prog.totalFiles = totalFiles;
        fileProgress_[filename] = prog;
    }
    
    ExtractionProgress getExtractionProgress(const std::string& filename) const {
        auto it = fileProgress_.find(filename);
        if (it != fileProgress_.end()) {
            return it->second;
        }
        return ExtractionProgress();
    }
    
    ExtractionProgress getOverallProgress() const {
        ExtractionProgress overall;
        overall.currentFile = "Overall";
        overall.totalFiles = 0;
        overall.filesExtracted = 0;
        
        for (const auto& [filename, progress] : fileProgress_) {
            overall.totalFiles += progress.totalFiles;
            overall.filesExtracted += progress.filesExtracted;
        }
        
        overall.percentage = (overall.totalFiles > 0) 
            ? static_cast<float>(overall.filesExtracted) / static_cast<float>(overall.totalFiles)
            : 0.0f;
            
        return overall;
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
    
    void updateStorageInfo(size_t totalSpace, size_t usedSpace) {
        storageInfo_.totalSpace = totalSpace;
        storageInfo_.usedSpace = usedSpace;
        storageInfo_.availableSpace = totalSpace - usedSpace;
        storageInfo_.usagePercentage = (totalSpace > 0) 
            ? (static_cast<float>(usedSpace) / static_cast<float>(totalSpace) * 100.0f) 
            : 0.0f;
    }
    
    StorageInfo getStorageInfo() const {
        return storageInfo_;
    }
    
    bool hasEnoughSpaceForExtraction(size_t requiredSpace) const {
        return storageInfo_.availableSpace >= requiredSpace;
    }
    
private:
    bool initialized_ = false;
    std::unordered_map<std::string, ExtractionProgress> fileProgress_;
    ValidationStatus validationStatus_;
    StorageInfo storageInfo_;
};

} // namespace ui
} // namespace d2