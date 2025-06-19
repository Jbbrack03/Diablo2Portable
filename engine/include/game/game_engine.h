#pragma once

namespace d2 {

class GameEngine {
public:
    GameEngine() = default;
    ~GameEngine() = default;
    
    bool initialize() {
        initialized_ = true;
        return true;
    }
    
    bool isInitialized() const { return initialized_; }
    bool isRunning() const { return running_; }
    
private:
    bool initialized_ = false;
    bool running_ = false;
};

} // namespace d2