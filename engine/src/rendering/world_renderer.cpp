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
#include <unordered_set>

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
            // Get texture ID for player based on character class
            std::string spriteName = getSpriteName(player->getCharacterClass());
            uint32_t playerTextureId = loadOrGetSprite(spriteName);
            
            // Map entity to texture
            entityTextureMap_[player->getId()] = playerTextureId;
            
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
            // Get texture ID for monster based on type
            std::string monsterSpriteName = getMonsterSpriteName(monster->getType());
            uint32_t monsterTextureId = loadOrGetSprite(monsterSpriteName);
            
            // Map entity to texture
            entityTextureMap_[id] = monsterTextureId;
            
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

bool WorldRenderer::hasLoadedSprite(const std::string& spriteName) const {
    return spriteCache_.find(spriteName) != spriteCache_.end();
}

uint32_t WorldRenderer::getTextureIdForEntity(d2::game::EntityId entityId) const {
    auto it = entityTextureMap_.find(entityId);
    if (it != entityTextureMap_.end()) {
        return it->second;
    }
    return 0; // No texture loaded
}

void WorldRenderer::cleanupUnusedSprites() {
    // Collect all textures still in use
    std::unordered_set<uint32_t> usedTextures;
    for (const auto& [entityId, textureId] : entityTextureMap_) {
        usedTextures.insert(textureId);
    }
    
    // Remove unused sprites from cache
    auto it = spriteCache_.begin();
    while (it != spriteCache_.end()) {
        if (usedTextures.find(it->second) == usedTextures.end()) {
            it = spriteCache_.erase(it);
        } else {
            ++it;
        }
    }
}

void WorldRenderer::removeEntityTexture(d2::game::EntityId entityId) {
    entityTextureMap_.erase(entityId);
}

// Helper method to get sprite name for character class
std::string WorldRenderer::getSpriteName(d2::game::CharacterClass charClass) const {
    switch (charClass) {
        case d2::game::CharacterClass::BARBARIAN:
            return "barbarian_walk";
        case d2::game::CharacterClass::AMAZON:
            return "amazon_walk";
        case d2::game::CharacterClass::SORCERESS:
            return "sorceress_walk";
        case d2::game::CharacterClass::NECROMANCER:
            return "necromancer_walk";
        case d2::game::CharacterClass::PALADIN:
            return "paladin_walk";
        case d2::game::CharacterClass::ASSASSIN:
            return "assassin_walk";
        case d2::game::CharacterClass::DRUID:
            return "druid_walk";
        default:
            return "barbarian_walk";
    }
}

// Helper method to get sprite name for monster type
std::string WorldRenderer::getMonsterSpriteName(d2::game::MonsterType type) const {
    switch (type) {
        case d2::game::MonsterType::SKELETON:
            return "skeleton_walk";
        case d2::game::MonsterType::ZOMBIE:
            return "zombie_walk";
        case d2::game::MonsterType::DEMON:
            return "demon_walk";
        case d2::game::MonsterType::FALLEN:
            return "fallen_walk";
        case d2::game::MonsterType::GOLEM:
            return "golem_walk";
        default:
            return "zombie_walk";
    }
}

// Helper method to load or get cached sprite
uint32_t WorldRenderer::loadOrGetSprite(const std::string& spriteName) {
    // Check if already cached
    auto it = spriteCache_.find(spriteName);
    if (it != spriteCache_.end()) {
        return it->second;
    }
    
    // Load sprite (for now, use placeholder IDs)
    // In real implementation, this would load from asset manager
    static uint32_t nextTextureId = 1000;
    uint32_t textureId = nextTextureId++;
    
    // Cache it
    spriteCache_[spriteName] = textureId;
    
    return textureId;
}

} // namespace d2::rendering