#include "rendering/optimized_world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/camera.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "map/map_loader.h"
#include "core/asset_manager.h"
#include <algorithm>

namespace d2::rendering {

OptimizedWorldRenderer::OptimizedWorldRenderer() 
    : monsterGrid_(std::make_unique<d2::performance::SpatialGrid<d2::game::Monster>>(128.0f)) {
}

void OptimizedWorldRenderer::initialize(const d2portable::core::AssetManager& assetManager) {
    WorldRenderer::initialize(assetManager);
}

void OptimizedWorldRenderer::render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer) {
    if (!optimizationsEnabled_) {
        // Fall back to base implementation
        WorldRenderer::render(gameState, spriteRenderer);
        return;
    }
    
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
    // Reset statistics
    lastRenderedEntityCount_ = 0;
    lastCulledEntityCount_ = 0;
    
    // For non-camera rendering, use a default viewport
    // Assume 800x600 centered at origin for now
    viewportMin_ = glm::vec2(-400, -300);
    viewportMax_ = glm::vec2(400, 300);
    
    // Update spatial grid with current monster positions
    updateSpatialGrid(gameState);
    
    // Render map tiles if present (already optimized in base class)
    if (gameState.hasMap()) {
        const auto* map = gameState.getMap();
        if (map) {
            uint32_t tileTextureId = assetManager_ ? 200 : 2;
            const float TILE_SIZE = 32.0f;
            
            // Only render visible tiles
            int startX = std::max(0, static_cast<int>(viewportMin_.x / TILE_SIZE) - 1);
            int endX = std::min(map->getWidth(), static_cast<int>(viewportMax_.x / TILE_SIZE) + 2);
            int startY = std::max(0, static_cast<int>(viewportMin_.y / TILE_SIZE) - 1);
            int endY = std::min(map->getHeight(), static_cast<int>(viewportMax_.y / TILE_SIZE) + 2);
            
            for (int y = startY; y < endY; y++) {
                for (int x = startX; x < endX; x++) {
                    glm::vec2 tilePos(x * TILE_SIZE, y * TILE_SIZE);
                    spriteRenderer.drawSprite(tileTextureId, tilePos, glm::vec2(TILE_SIZE, TILE_SIZE));
                }
            }
        }
    }
    
    // Render player if present
    if (gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            uint32_t playerTextureId = assetManager_ ? 100 : 1;
            const glm::vec2 PLAYER_SIZE(64.0f, 64.0f);
            
            spriteRenderer.drawSprite(playerTextureId, player->getPosition(), PLAYER_SIZE);
            lastRenderedEntityCount_++;
        }
    }
    
    // Render only visible monsters using spatial grid
    auto visibleMonsters = monsterGrid_->getEntitiesInBounds(viewportMin_, viewportMax_);
    
    for (const auto& [id, monster] : visibleMonsters) {
        if (monster && isInViewport(monster->getPosition(), glm::vec2(48.0f, 48.0f))) {
            uint32_t monsterTextureId = 3;
            if (assetManager_) {
                switch (static_cast<int>(monster->getType())) {
                    case static_cast<int>(d2::game::MonsterType::SKELETON): monsterTextureId = 400; break;
                    case static_cast<int>(d2::game::MonsterType::ZOMBIE): monsterTextureId = 401; break;
                    case static_cast<int>(d2::game::MonsterType::DEMON): monsterTextureId = 402; break;
                    case static_cast<int>(d2::game::MonsterType::FALLEN): monsterTextureId = 403; break;
                    case static_cast<int>(d2::game::MonsterType::GOLEM): monsterTextureId = 404; break;
                }
            }
            
            const glm::vec2 MONSTER_SIZE(48.0f, 48.0f);
            spriteRenderer.drawSprite(monsterTextureId, monster->getPosition(), MONSTER_SIZE);
            lastRenderedEntityCount_++;
        } else {
            lastCulledEntityCount_++;
        }
    }
    
    // Render HUD if enabled
    if (isHUDEnabled() && gameState.hasPlayer()) {
        // HUD rendering (same as base class)
        const uint32_t HEALTH_HUD_TEXTURE = 300;
        const uint32_t MANA_HUD_TEXTURE = 301;
        const glm::vec2 HEALTH_POS(50.0f, 550.0f);
        const glm::vec2 MANA_POS(650.0f, 550.0f);
        const glm::vec2 HUD_SIZE(100.0f, 30.0f);
        
        spriteRenderer.drawSprite(HEALTH_HUD_TEXTURE, HEALTH_POS, HUD_SIZE);
        spriteRenderer.drawSprite(MANA_HUD_TEXTURE, MANA_POS, HUD_SIZE);
    }
    
    spriteRenderer.endFrame();
}

void OptimizedWorldRenderer::renderWithCamera(const d2::game::GameState& gameState, 
                                              SpriteRenderer& spriteRenderer, 
                                              const Camera& camera) {
    if (!optimizationsEnabled_) {
        // Fall back to base implementation
        WorldRenderer::renderWithCamera(gameState, spriteRenderer, camera);
        return;
    }
    
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
    // Reset statistics
    lastRenderedEntityCount_ = 0;
    lastCulledEntityCount_ = 0;
    
    // Calculate viewport bounds from camera
    glm::vec2 cameraCenter = camera.getCenter();
    const float viewportWidth = 800.0f;  // Should get from camera
    const float viewportHeight = 600.0f;
    
    viewportMin_ = cameraCenter - glm::vec2(viewportWidth / 2.0f, viewportHeight / 2.0f);
    viewportMax_ = cameraCenter + glm::vec2(viewportWidth / 2.0f, viewportHeight / 2.0f);
    
    // Update spatial grid
    updateSpatialGrid(gameState);
    
    // Render map tiles with culling (same as base class)
    if (gameState.hasMap()) {
        const auto* map = gameState.getMap();
        if (map) {
            uint32_t tileTextureId = assetManager_ ? 200 : 2;
            const float TILE_SIZE = 32.0f;
            
            int startX = std::max(0, static_cast<int>(viewportMin_.x / TILE_SIZE) - 1);
            int endX = std::min(map->getWidth(), static_cast<int>(viewportMax_.x / TILE_SIZE) + 2);
            int startY = std::max(0, static_cast<int>(viewportMin_.y / TILE_SIZE) - 1);
            int endY = std::min(map->getHeight(), static_cast<int>(viewportMax_.y / TILE_SIZE) + 2);
            
            for (int y = startY; y < endY; y++) {
                for (int x = startX; x < endX; x++) {
                    glm::vec2 tilePos(x * TILE_SIZE, y * TILE_SIZE);
                    spriteRenderer.drawSprite(tileTextureId, tilePos, glm::vec2(TILE_SIZE, TILE_SIZE));
                }
            }
        }
    }
    
    // Render player
    if (gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            uint32_t playerTextureId = assetManager_ ? 100 : 1;
            const glm::vec2 PLAYER_SIZE(64.0f, 64.0f);
            spriteRenderer.drawSprite(playerTextureId, player->getPosition(), PLAYER_SIZE);
            lastRenderedEntityCount_++;
        }
    }
    
    // Render only visible monsters
    auto visibleMonsters = monsterGrid_->getEntitiesInBounds(viewportMin_, viewportMax_);
    
    for (const auto& [id, monster] : visibleMonsters) {
        if (monster && isInViewport(monster->getPosition(), glm::vec2(48.0f, 48.0f))) {
            uint32_t monsterTextureId = 3;
            if (assetManager_) {
                switch (static_cast<int>(monster->getType())) {
                    case static_cast<int>(d2::game::MonsterType::SKELETON): monsterTextureId = 400; break;
                    case static_cast<int>(d2::game::MonsterType::ZOMBIE): monsterTextureId = 401; break;
                    case static_cast<int>(d2::game::MonsterType::DEMON): monsterTextureId = 402; break;
                    case static_cast<int>(d2::game::MonsterType::FALLEN): monsterTextureId = 403; break;
                    case static_cast<int>(d2::game::MonsterType::GOLEM): monsterTextureId = 404; break;
                }
            }
            
            const glm::vec2 MONSTER_SIZE(48.0f, 48.0f);
            spriteRenderer.drawSprite(monsterTextureId, monster->getPosition(), MONSTER_SIZE);
            lastRenderedEntityCount_++;
        } else {
            lastCulledEntityCount_++;
        }
    }
    
    spriteRenderer.endFrame();
}

void OptimizedWorldRenderer::updateSpatialGrid(const d2::game::GameState& gameState) {
    monsterGrid_->clear();
    
    const auto& monsters = gameState.getAllMonsters();
    for (const auto& [id, monster] : monsters) {
        if (monster) {
            monsterGrid_->add(id, monster->getPosition(), monster.get());
        }
    }
}

bool OptimizedWorldRenderer::isInViewport(const glm::vec2& position, const glm::vec2& size) const {
    // Check if entity bounds intersect with viewport
    glm::vec2 entityMin = position - size * 0.5f;
    glm::vec2 entityMax = position + size * 0.5f;
    
    return entityMax.x >= viewportMin_.x && entityMin.x <= viewportMax_.x &&
           entityMax.y >= viewportMin_.y && entityMin.y <= viewportMax_.y;
}

} // namespace d2::rendering