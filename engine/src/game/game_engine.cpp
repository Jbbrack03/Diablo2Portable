#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "rendering/renderer.h"
#include "game/game_state.h"

namespace d2 {

GameEngine::GameEngine() = default;

GameEngine::~GameEngine() = default;

bool GameEngine::initialize(const std::string& assetPath) {
    if (initialized_) {
        return true;
    }
    
    // Create asset manager
    assetManager_ = std::make_unique<d2portable::core::AssetManager>();
    
    // Initialize asset manager with provided path
    if (!assetPath.empty()) {
        if (!assetManager_->initialize(assetPath)) {
            return false;
        }
    }
    
    // Create renderer
    renderer_ = std::make_unique<d2::rendering::Renderer>();
    
    // Create game state
    gameState_ = std::make_unique<d2::game::GameState>();
    
    initialized_ = true;
    return true;
}

bool GameEngine::start() {
    if (!initialized_) {
        return false;
    }
    
    if (running_) {
        return true;
    }
    
    running_ = true;
    return true;
}

void GameEngine::stop() {
    running_ = false;
}

bool GameEngine::renderFrame() {
    if (!initialized_ || !running_) {
        return false;
    }
    
    // TODO: Implement actual rendering logic
    // For now, just return true to indicate successful frame render
    return true;
}

} // namespace d2