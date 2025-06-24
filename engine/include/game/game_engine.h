#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "game/entity_manager.h"

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
class CombatEngine;
class LootSystem;
}
class QuestManager;
namespace input {
class InputManager;
class TouchInput;
enum class TouchAction;
}
namespace performance {
class PerformanceMonitor;
class OptimizedUpdateSystem;
}

class GameEngine {
public:
    enum class TouchControlMode {
        DIRECT_MOVEMENT,
        VIRTUAL_JOYSTICK
    };
    
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
    void processCombat(float deltaTime);
    void processMonsterDeath(game::EntityId monsterId);
    void processItemPickup();
    
    // Touch input support
    void processTouchInput(float x, float y, input::TouchAction action);
    void setTouchControlMode(TouchControlMode mode);
    void setScreenSize(int width, int height);
    bool wasActionTriggered() const { return actionTriggered_; }
    
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
    
    d2::game::LootSystem* getLootSystem() const {
        return lootSystem_.get();
    }
    
    QuestManager* getQuestManager() const {
        return questManager_.get();
    }
    
    d2::performance::PerformanceMonitor* getPerformanceMonitor() const {
        return performanceMonitor_.get();
    }
    
    d2::performance::OptimizedUpdateSystem* getOptimizedUpdateSystem() const {
        return optimizedUpdateSystem_.get();
    }
    
    // Enable/disable optimizations
    void setOptimizationsEnabled(bool enabled);
    
private:
    bool initialized_ = false;
    bool running_ = false;
    bool actionTriggered_ = false;
    TouchControlMode touchControlMode_ = TouchControlMode::DIRECT_MOVEMENT;
    std::unique_ptr<d2portable::core::AssetManager> assetManager_;
    std::unique_ptr<d2::rendering::Renderer> renderer_;
    std::unique_ptr<d2::rendering::WorldRenderer> worldRenderer_;
    std::unique_ptr<d2::rendering::Camera> camera_;
    std::unique_ptr<d2::rendering::SpriteRenderer> spriteRenderer_;
    std::unique_ptr<d2::game::GameState> gameState_;
    std::unique_ptr<d2::input::InputManager> inputManager_;
    std::unique_ptr<d2::input::TouchInput> touchInput_;
    std::unique_ptr<d2::game::CombatEngine> combatEngine_;
    std::unique_ptr<d2::game::LootSystem> lootSystem_;
    std::unique_ptr<QuestManager> questManager_;
    std::unique_ptr<d2::performance::PerformanceMonitor> performanceMonitor_;
    std::unique_ptr<d2::performance::OptimizedUpdateSystem> optimizedUpdateSystem_;
};

} // namespace d2