#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "rendering/renderer.h"

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
    
    initialized_ = true;
    return true;
}

} // namespace d2