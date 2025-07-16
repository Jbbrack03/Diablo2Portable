#include "game/game_engine.h"
#include "core/asset_manager.h"
#include "rendering/renderer.h"
#include "rendering/world_renderer.h"
#include "rendering/optimized_world_renderer.h"
#include "rendering/camera.h"
#include "rendering/sprite_renderer.h"
#include "rendering/texture_manager.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/combat_engine.h"
#include "game/loot_system.h"
#include "game/dropped_item.h"
#include "game/quest_manager.h"
#include "input/input_manager.h"
#include "input/touch_input.h"
#include "performance/performance_monitor.h"
#include "performance/optimized_update_system.h"
#include "android/asset_path_validator.h"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace d2 {

GameEngine::GameEngine() = default;

GameEngine::~GameEngine() = default;

bool GameEngine::initialize(const std::string& assetPath) {
    if (initialized_) {
        return true;
    }
    
    // Initialize components in order
    if (!initializeAssetManager(assetPath)) {
        return false;
    }
    
    if (!initializeRenderingComponents()) {
        return false;
    }
    
    if (!initializeGameComponents()) {
        return false;
    }
    
    if (!initializeInputComponents()) {
        return false;
    }
    
    if (!initializePerformanceComponents()) {
        return false;
    }
    
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
    
    // Start performance timing
    if (performanceMonitor_) {
        performanceMonitor_->startFrame();
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
    
    // End performance timing
    if (performanceMonitor_) {
        performanceMonitor_->endFrame();
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
    
    // Process touch input if we have ongoing touch
    if (touchInput_ && touchInput_->isTouching()) {
        auto gameInput = touchInput_->getGameInput();
        if (gameInput.isMoving && gameState_ && gameState_->hasPlayer()) {
            processInput(gameInput.moveDirection);
        }
    }
    
    // Update game state (physics, AI, animations, etc.)
    if (optimizedUpdateSystem_ && gameState_) {
        // Use optimized update system for entities
        optimizedUpdateSystem_->updateEntities(*gameState_, deltaTime);
    }
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
    if (!isValidCombatScenario()) {
        return;
    }
    
    auto player = gameState_->getPlayer();
    glm::vec2 playerPos = player->getPosition();
    
    // Process combat for each monster
    const auto& monsters = gameState_->getAllMonsters();
    for (const auto& [id, monster] : monsters) {
        if (!monster || monster->getCurrentLife() <= 0) {
            continue;
        }
        
        if (isMonsterInMeleeRange(monster, playerPos)) {
            processMeleeDamage(monster);
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

void GameEngine::processItemPickup() {
    if (!initialized_ || !gameState_) {
        return;
    }
    
    // Get the player
    auto player = gameState_->getPlayer();
    if (!player) {
        return;
    }
    
    // Get player position
    glm::vec2 playerPos = player->getPosition();
    
    // Define pickup radius
    const float PICKUP_RADIUS = 20.0f;
    
    // Check all dropped items
    const auto& droppedItems = gameState_->getAllDroppedItems();
    std::vector<game::EntityId> itemsToPickup;
    
    for (const auto& [id, droppedItem] : droppedItems) {
        // Calculate distance to item
        glm::vec2 itemPos = droppedItem->getPosition();
        float distance = glm::length(itemPos - playerPos);
        
        // If within pickup range, mark for pickup
        if (distance <= PICKUP_RADIUS) {
            itemsToPickup.push_back(id);
        }
    }
    
    // Remove picked up items from the world
    for (game::EntityId id : itemsToPickup) {
        // In a full implementation, we would:
        // 1. Get the item from the dropped item
        // 2. Add it to the player's inventory
        // 3. Only remove from world if successfully added to inventory
        
        // For now, just remove from world
        gameState_->removeDroppedItem(id);
    }
}

void GameEngine::setOptimizationsEnabled(bool enabled) {
    // Enable/disable optimizations in all systems
    if (worldRenderer_) {
        auto* optimizedRenderer = dynamic_cast<d2::rendering::OptimizedWorldRenderer*>(worldRenderer_.get());
        if (optimizedRenderer) {
            optimizedRenderer->setOptimizationsEnabled(enabled);
        }
    }
    
    if (optimizedUpdateSystem_) {
        optimizedUpdateSystem_->setOptimizationsEnabled(enabled);
    }
}

void GameEngine::processTouchInput(float x, float y, input::TouchAction action) {
    if (!touchInput_ || !gameState_ || !initialized_) {
        return;
    }
    
    // Clear action triggered flag at the start of each input
    if (action == input::TouchAction::DOWN) {
        actionTriggered_ = false;
    }
    
    // Process the touch event
    touchInput_->onTouchEvent(x, y, action);
    
    // Check for tap action
    if (touchInput_->wasTapped()) {
        actionTriggered_ = true;
        touchInput_->clearTapState();
    }
    
    // Convert touch input to game input
    auto gameInput = touchInput_->getGameInput();
    
    // Process movement if touching
    if (gameInput.isMoving && gameState_->getPlayer()) {
        // Convert to movement input for the existing system
        processInput(gameInput.moveDirection);
    }
    
    // Handle action button
    if (gameInput.actionPressed) {
        actionTriggered_ = true;
        // In a full implementation, this would trigger the player's primary action
    }
}

void GameEngine::setTouchControlMode(TouchControlMode mode) {
    touchControlMode_ = mode;
    
    if (touchInput_) {
        touchInput_->enableVirtualJoystick(mode == TouchControlMode::VIRTUAL_JOYSTICK);
    }
}

void GameEngine::setScreenSize(int width, int height) {
    if (touchInput_) {
        touchInput_->setScreenSize(width, height);
    }
    
    // Also update camera/renderer if needed
    if (camera_) {
        // camera_->setViewportSize(width, height);
    }
}

bool GameEngine::initializeAssetManager(const std::string& assetPath) {
    // Create asset manager
    assetManager_ = std::make_unique<d2portable::core::AssetManager>();
    
    // Initialize asset manager with provided path
    if (!assetPath.empty()) {
        // Check if path exists before trying to detect MPQ files
        if (!std::filesystem::exists(assetPath)) {
            // Path doesn't exist, but we can still initialize with default settings
            return assetManager_->initialize(assetPath);
        }
        
        // Initialize with appropriate method based on content
        if (detectMPQFiles(assetPath)) {
            // Only validate MPQ files if they are present
            auto validationResult = AssetPathValidator::validateAssetPath(assetPath);
            if (!validationResult.isValid) {
                // Log error details (in production, this would go to proper logging)
                // For now, just fail initialization if required assets are missing
                return false;
            }
            return assetManager_->initializeWithMPQs(assetPath);
        } else {
            // No MPQ files detected, initialize with basic asset loading
            return assetManager_->initialize(assetPath);
        }
    }
    
    return true;
}

bool GameEngine::detectMPQFiles(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        return false;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
            if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".mpq") {
                return true;
            }
        }
    }
    
    return false;
}

bool GameEngine::initializeRenderingComponents() {
    // Create renderer
    renderer_ = std::make_unique<d2::rendering::Renderer>();
    
    // Create sprite renderer
    spriteRenderer_ = std::make_unique<d2::rendering::SpriteRenderer>();
    // Initialize sprite renderer (simplified for now)
    d2::rendering::TextureManager textureManager;
    spriteRenderer_->initialize(*renderer_, textureManager);
    
    // Create optimized world renderer
    worldRenderer_ = std::make_unique<d2::rendering::OptimizedWorldRenderer>();
    
    // Create camera (default 800x600 for now)
    camera_ = std::make_unique<d2::rendering::Camera>(800, 600);
    
    return true;
}

bool GameEngine::initializeGameComponents() {
    // Create game state
    gameState_ = std::make_unique<d2::game::GameState>();
    
    // Create combat engine
    combatEngine_ = std::make_unique<d2::game::CombatEngine>();
    
    // Create loot system
    lootSystem_ = std::make_unique<d2::game::LootSystem>();
    
    // Create quest manager
    questManager_ = std::make_unique<d2::QuestManager>();
    
    return true;
}

bool GameEngine::initializeInputComponents() {
    // Create input manager (with nullptr for now - will be properly initialized later)
    inputManager_ = std::make_unique<d2::input::InputManager>(nullptr);
    
    // Create touch input system
    touchInput_ = std::make_unique<d2::input::TouchInput>();
    touchInput_->setScreenSize(800, 600); // Default size, will be updated
    
    return true;
}

bool GameEngine::initializePerformanceComponents() {
    // Create performance monitor
    performanceMonitor_ = std::make_unique<d2::performance::PerformanceMonitor>();
    
    // Create optimized update system
    optimizedUpdateSystem_ = std::make_unique<d2::performance::OptimizedUpdateSystem>();
    
    return true;
}

void GameEngine::updateInput() {
    if (!inputManager_) {
        return;
    }
    
    inputManager_->update();
    
    // Process player movement from input
    if (gameState_ && gameState_->hasPlayer()) {
        glm::vec2 movement = inputManager_->getMovement();
        processInput(movement);
    }
}

void GameEngine::updateTouchInput(float deltaTime) {
    if (!touchInput_ || !touchInput_->isTouching()) {
        return;
    }
    
    auto gameInput = touchInput_->getGameInput();
    if (gameInput.isMoving && gameState_ && gameState_->hasPlayer()) {
        processInput(gameInput.moveDirection);
    }
}

void GameEngine::updateEntitySystems(float deltaTime) {
    if (!optimizedUpdateSystem_ || !gameState_) {
        return;
    }
    
    // Use optimized update system for entities
    optimizedUpdateSystem_->updateEntities(*gameState_, deltaTime);
}

bool GameEngine::isValidCombatScenario() const {
    return initialized_ && running_ && gameState_ && combatEngine_ &&
           gameState_->hasPlayer() && gameState_->getPlayer();
}

bool GameEngine::isMonsterInMeleeRange(const std::shared_ptr<game::Monster>& monster, 
                                       const glm::vec2& playerPos) const {
    const float MELEE_RANGE = 50.0f;
    glm::vec2 monsterPos = monster->getPosition();
    float distance = glm::length(monsterPos - playerPos);
    return distance <= MELEE_RANGE;
}

void GameEngine::processMeleeDamage(const std::shared_ptr<game::Monster>& monster) {
    // For now, simple combat processing
    // In a full implementation, we'd get player stats from character
    // and use CombatEngine to calculate hit chance and damage
    
    // Placeholder combat - just do a fixed damage
    const int MELEE_DAMAGE = 10;
    monster->takeDamage(MELEE_DAMAGE);
}

} // namespace d2