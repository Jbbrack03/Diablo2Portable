#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/camera.h"
#include "game/game_state.h"
#include "game/player.h"
#include "map/map_loader.h"
#include "core/asset_manager.h"
#include <algorithm>

namespace d2::rendering {

void WorldRenderer::initialize(const d2portable::core::AssetManager& assetManager) {
    assetManager_ = &assetManager;
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
                // In a real implementation, we'd look up the player sprite based on class/animation
                // For now, use a fixed ID that's higher than placeholders
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

} // namespace d2::rendering