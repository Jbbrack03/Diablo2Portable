#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "rendering/renderer.h"
#include "rendering/world_renderer.h"
#include "rendering/camera.h"
#include "rendering/sprite_renderer.h"
#include "rendering/texture_manager.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/combat_engine.h"
#include "game/loot_system.h"
#include "game/dropped_item.h"
#include "input/input_manager.h"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <cstdlib>

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
    
    // Create combat engine
    combatEngine_ = std::make_unique<d2::game::CombatEngine>();
    
    // Create loot system
    lootSystem_ = std::make_unique<d2::game::LootSystem>();
    
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

void GameEngine::processCombat(float deltaTime) {
    if (!initialized_ || !running_ || !gameState_ || !combatEngine_) {
        return;
    }
    
    // Only process combat if we have a player
    if (!gameState_->hasPlayer()) {
        return;
    }
    
    auto player = gameState_->getPlayer();
    if (!player) {
        return;
    }
    
    // Check combat between player and all monsters
    const auto& monsters = gameState_->getAllMonsters();
    for (const auto& [id, monster] : monsters) {
        if (!monster || monster->getCurrentLife() <= 0) {
            continue;
        }
        
        // Calculate distance between player and monster
        glm::vec2 playerPos = player->getPosition();
        glm::vec2 monsterPos = monster->getPosition();
        float distance = glm::length(monsterPos - playerPos);
        
        // If within melee range, process combat
        const float MELEE_RANGE = 50.0f;
        if (distance <= MELEE_RANGE) {
            // For now, simple combat processing
            // In a full implementation, we'd get player stats from character
            // and use CombatEngine to calculate hit chance and damage
            
            // Placeholder combat - just do a fixed damage
            monster->takeDamage(10);
        }
    }
}

void GameEngine::processMonsterDeath(game::EntityId monsterId) {
    if (!initialized_ || !gameState_ || !lootSystem_) {
        return;
    }
    
    // Get the monster
    auto monster = gameState_->getMonster(monsterId);
    if (!monster) {
        return;
    }
    
    // Generate loot using the loot system
    auto lootItems = lootSystem_->generateLoot(monster);
    
    // Add each dropped item to the game world
    for (const auto& item : lootItems) {
        // Calculate drop position near the monster
        glm::vec2 monsterPos = monster->getPosition();
        
        // Add some random offset so items don't all stack on top of each other
        float offsetX = (rand() % 40) - 20;  // -20 to +20
        float offsetY = (rand() % 40) - 20;  // -20 to +20
        glm::vec2 dropPos = monsterPos + glm::vec2(offsetX, offsetY);
        
        // Create a dropped item entity
        auto droppedItem = std::make_shared<game::DroppedItem>(item, dropPos);
        
        // Add it to the game state
        gameState_->addDroppedItem(droppedItem);
    }
}

} // namespace d2