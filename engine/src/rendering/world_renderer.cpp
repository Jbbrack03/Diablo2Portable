#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/sprite_animation.h"
#include "rendering/camera.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "map/map_loader.h"
#include "core/asset_manager.h"
#include <algorithm>

namespace d2::rendering {

void WorldRenderer::initialize(const d2portable::core::AssetManager& assetManager) {
    assetManager_ = &assetManager;
}

void WorldRenderer::setHUDEnabled(bool enabled) {
    hudEnabled_ = enabled;
}

bool WorldRenderer::isHUDEnabled() const {
    return hudEnabled_;
}

void WorldRenderer::render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer) {
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
    // Render map tiles if present
    if (gameState.hasMap()) {
        const auto* map = gameState.getMap();
        if (map) {
            // Use texture from asset manager if available, otherwise placeholder
            uint32_t tileTextureId = 2; // Default placeholder
            if (assetManager_) {
                // In a real implementation, we'd look up the tile texture
                // For now, use a fixed ID that's higher than placeholders
                tileTextureId = 200;
            }
            
            const float TILE_SIZE = 32.0f; // Standard tile size
            
            // Render all tiles in the map
            for (int y = 0; y < map->getHeight(); y++) {
                for (int x = 0; x < map->getWidth(); x++) {
                    // Calculate tile position
                    glm::vec2 tilePos(x * TILE_SIZE, y * TILE_SIZE);
                    
                    // Draw tile sprite
                    spriteRenderer.drawSprite(
                        tileTextureId,
                        tilePos,
                        glm::vec2(TILE_SIZE, TILE_SIZE)
                    );
                }
            }
        }
    }
    
    // Render player if present
    if (gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            // Use texture from asset manager if available
            uint32_t playerTextureId = 1; // Default placeholder
            if (assetManager_) {
                // Look up animation for this entity
                auto animationIter = entityAnimations_.find(player->getId());
                if (animationIter != entityAnimations_.end()) {
                    const SpriteAnimation& animation = animationIter->second;
                    // Calculate texture ID based on animation frame and direction
                    // Base texture ID + current frame + (direction * frame_count)
                    uint32_t baseTextureId = 100; // Base player texture
                    uint32_t frameOffset = animation.getCurrentFrame();
                    uint32_t directionOffset = animation.getCurrentDirection() * 8; // Assuming 8 frames per direction
                    playerTextureId = baseTextureId + frameOffset + directionOffset;
                } else {
                    // Use static texture if no animation
                    playerTextureId = 100;
                }
            }
            
            const glm::vec2 PLAYER_SIZE(64.0f, 64.0f);
            
            spriteRenderer.drawSprite(
                playerTextureId,
                player->getPosition(),
                PLAYER_SIZE
            );
        }
    }
    
    // Render monsters
    const auto& monsters = gameState.getAllMonsters();
    for (const auto& [id, monster] : monsters) {
        if (monster) {
            // Use texture based on monster type
            uint32_t monsterTextureId = 3; // Default placeholder
            if (assetManager_) {
                // Different texture IDs for different monster types
                switch (monster->getType()) {
                    case d2::game::MonsterType::SKELETON:
                        monsterTextureId = 400;
                        break;
                    case d2::game::MonsterType::ZOMBIE:
                        monsterTextureId = 401;
                        break;
                    case d2::game::MonsterType::DEMON:
                        monsterTextureId = 402;
                        break;
                    case d2::game::MonsterType::FALLEN:
                        monsterTextureId = 403;
                        break;
                    case d2::game::MonsterType::GOLEM:
                        monsterTextureId = 404;
                        break;
                }
            }
            
            const glm::vec2 MONSTER_SIZE(48.0f, 48.0f);
            
            spriteRenderer.drawSprite(
                monsterTextureId,
                monster->getPosition(),
                MONSTER_SIZE
            );
        }
    }
    
    // Render HUD if enabled and player exists
    if (hudEnabled_ && gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            // For now, just render the HUD elements without actual stats
            // In a real implementation, we'd get health/mana values from the character
            
            // Health orb/bar (bottom left)
            const uint32_t HEALTH_HUD_TEXTURE = 300;
            const glm::vec2 HEALTH_POS(50.0f, 550.0f); // Bottom left
            const glm::vec2 HUD_SIZE(100.0f, 30.0f);
            
            spriteRenderer.drawSprite(
                HEALTH_HUD_TEXTURE,
                HEALTH_POS,
                HUD_SIZE
            );
            
            // Mana orb/bar (bottom right) 
            const uint32_t MANA_HUD_TEXTURE = 301;
            const glm::vec2 MANA_POS(650.0f, 550.0f); // Bottom right
            
            spriteRenderer.drawSprite(
                MANA_HUD_TEXTURE,
                MANA_POS,
                HUD_SIZE
            );
        }
    }
    
    // End rendering frame
    spriteRenderer.endFrame();
}

void WorldRenderer::renderWithCamera(const d2::game::GameState& gameState, 
                                     SpriteRenderer& spriteRenderer, 
                                     const Camera& camera) {
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
    // Render map tiles if present with viewport culling
    if (gameState.hasMap()) {
        const auto* map = gameState.getMap();
        if (map) {
            // Use texture from asset manager if available
            uint32_t tileTextureId = 2; // Default placeholder
            if (assetManager_) {
                tileTextureId = 200;
            }
            const float TILE_SIZE = 32.0f;
            
            // Get camera center and calculate visible bounds
            glm::vec2 cameraCenter = camera.getCenter();
            
            // Calculate viewport bounds in tile coordinates
            // Assuming 800x600 viewport for now (should get from camera)
            const float viewportWidth = 800.0f;
            const float viewportHeight = 600.0f;
            
            float leftBound = cameraCenter.x - viewportWidth / 2.0f;
            float rightBound = cameraCenter.x + viewportWidth / 2.0f;
            float topBound = cameraCenter.y - viewportHeight / 2.0f;
            float bottomBound = cameraCenter.y + viewportHeight / 2.0f;
            
            // Convert to tile indices with some margin
            int startX = std::max(0, static_cast<int>(leftBound / TILE_SIZE) - 1);
            int endX = std::min(map->getWidth(), static_cast<int>(rightBound / TILE_SIZE) + 2);
            int startY = std::max(0, static_cast<int>(topBound / TILE_SIZE) - 1);
            int endY = std::min(map->getHeight(), static_cast<int>(bottomBound / TILE_SIZE) + 2);
            
            // Only render visible tiles
            for (int y = startY; y < endY; y++) {
                for (int x = startX; x < endX; x++) {
                    glm::vec2 tilePos(x * TILE_SIZE, y * TILE_SIZE);
                    
                    spriteRenderer.drawSprite(
                        tileTextureId,
                        tilePos,
                        glm::vec2(TILE_SIZE, TILE_SIZE)
                    );
                }
            }
        }
    }
    
    // Render player if present
    if (gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            // Use texture from asset manager if available
            uint32_t playerTextureId = 1; // Default placeholder
            if (assetManager_) {
                playerTextureId = 100;
            }
            const glm::vec2 PLAYER_SIZE(64.0f, 64.0f);
            
            spriteRenderer.drawSprite(
                playerTextureId,
                player->getPosition(),
                PLAYER_SIZE
            );
        }
    }
    
    // End rendering frame
    spriteRenderer.endFrame();
}

void WorldRenderer::setEntityAnimation(d2::game::EntityId entityId, const SpriteAnimation& animation) {
    entityAnimations_.insert_or_assign(entityId, animation);
}

void WorldRenderer::updateAnimations(float deltaTime) {
    for (auto& [entityId, animation] : entityAnimations_) {
        animation.update(deltaTime);
    }
}

} // namespace d2::rendering