#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "rendering/renderer.h"
#include "rendering/world_renderer.h"
#include "rendering/camera.h"
#include "rendering/sprite_renderer.h"
#include "rendering/texture_manager.h"
#include "game/game_state.h"
#include "game/player.h"
#include "input/input_manager.h"

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
    
    // Create sprite renderer
    spriteRenderer_ = std::make_unique<d2::rendering::SpriteRenderer>();
    // Initialize sprite renderer (simplified for now)
    d2::rendering::TextureManager textureManager;
    spriteRenderer_->initialize(*renderer_, textureManager);
    
    // Create world renderer
    worldRenderer_ = std::make_unique<d2::rendering::WorldRenderer>();
    
    // Create camera (default 800x600 for now)
    camera_ = std::make_unique<d2::rendering::Camera>(800, 600);
    
    // Create game state
    gameState_ = std::make_unique<d2::game::GameState>();
    
    // Create input manager (with nullptr for now - will be properly initialized later)
    inputManager_ = std::make_unique<d2::input::InputManager>(nullptr);
    
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
    
    // Basic game loop structure:
    // 1. Calculate delta time (for now using fixed timestep)
    float deltaTime = 0.016f; // 60 FPS
    
    // 2. Update game state (includes input processing)
    update(deltaTime);
    
    // 3. Update camera to follow player
    if (camera_ && gameState_ && gameState_->hasPlayer()) {
        auto player = gameState_->getPlayer();
        camera_->followTarget(player.get());
        camera_->update();
    }
    
    // 4. Render the frame
    if (worldRenderer_ && spriteRenderer_ && gameState_) {
        // Render the world using the world renderer
        worldRenderer_->render(*gameState_, *spriteRenderer_);
    }
    
    return true;
}

void GameEngine::update(float deltaTime) {
    if (!initialized_ || !running_) {
        return;
    }
    
    // Update input manager
    if (inputManager_) {
        inputManager_->update();
        
        // Process player movement from input
        if (gameState_ && gameState_->hasPlayer()) {
            glm::vec2 movement = inputManager_->getMovement();
            processInput(movement);
        }
    }
    
    // Update game state (physics, AI, animations, etc.)
    // TODO: Add more game state updates
}

void GameEngine::processInput(const glm::vec2& movement) {
    if (!initialized_ || !running_) {
        return;
    }
    
    // Move the player based on input
    if (gameState_ && gameState_->hasPlayer()) {
        auto player = gameState_->getPlayer();
        if (player) {
            // Simple movement - scale by a fixed speed for now
            const float PLAYER_SPEED = 5.0f;
            player->move(movement * PLAYER_SPEED);
        }
    }
}

} // namespace d2