#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace d2portable {
namespace core {
class AssetManager;
}
}

namespace d2 {
namespace rendering {
class Renderer;
class WorldRenderer;
class Camera;
class SpriteRenderer;
}
namespace game {
class GameState;
}
namespace input {
class InputManager;
}

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
    
    bool start();
    void stop();
    
    bool renderFrame();
    
    void update(float deltaTime);
    void processInput(const glm::vec2& movement);
    
    d2portable::core::AssetManager* getAssetManager() const { 
        return assetManager_.get(); 
    }
    
    d2::rendering::Renderer* getRenderer() const {
        return renderer_.get();
    }
    
    d2::game::GameState* getGameState() const {
        return gameState_.get();
    }
    
    d2::input::InputManager* getInputManager() const {
        return inputManager_.get();
    }
    
private:
    bool initialized_ = false;
    bool running_ = false;
    std::unique_ptr<d2portable::core::AssetManager> assetManager_;
    std::unique_ptr<d2::rendering::Renderer> renderer_;
    std::unique_ptr<d2::rendering::WorldRenderer> worldRenderer_;
    std::unique_ptr<d2::rendering::Camera> camera_;
    std::unique_ptr<d2::rendering::SpriteRenderer> spriteRenderer_;
    std::unique_ptr<d2::game::GameState> gameState_;
    std::unique_ptr<d2::input::InputManager> inputManager_;
};

} // namespace d2