#pragma once

#include <memory>
#include <string>

namespace d2portable {
namespace core {
class AssetManager;
}
}

namespace d2 {

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    bool initialize() {
        return initialize("");
    }
    
    bool initialize(const std::string& assetPath);
    
    bool isInitialized() const { return initialized_; }
    bool isRunning() const { return running_; }
    
    d2portable::core::AssetManager* getAssetManager() const { 
        return assetManager_.get(); 
    }
    
private:
    bool initialized_ = false;
    bool running_ = false;
    std::unique_ptr<d2portable::core::AssetManager> assetManager_;
};

} // namespace d2