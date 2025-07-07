#pragma once

#include <string>

namespace d2 {
namespace ui {

struct ExtractionProgress {
    std::string currentFile;
    float percentage = 0.0f;
    int filesExtracted = 0;
    int totalFiles = 0;
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
    
private:
    bool initialized_ = false;
    ExtractionProgress progress_;
};

} // namespace ui
} // namespace d2