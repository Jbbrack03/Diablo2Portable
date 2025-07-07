#pragma once

namespace d2 {
namespace ui {

class AssetManagementUI {
public:
    AssetManagementUI() : initialized_(true) {}
    ~AssetManagementUI() = default;
    
    bool isInitialized() const { return initialized_; }
    
private:
    bool initialized_ = false;
};

} // namespace ui
} // namespace d2