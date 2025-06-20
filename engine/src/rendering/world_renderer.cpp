#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "game/game_state.h"
#include "game/player.h"
#include "map/map_loader.h"

namespace d2::rendering {

void WorldRenderer::render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer) {
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
    // Render map tiles if present
    if (gameState.hasMap()) {
        const auto* map = gameState.getMap();
        if (map) {
            // For now, render each tile as a placeholder sprite
            const uint32_t TILE_TEXTURE_ID = 2;
            const float TILE_SIZE = 32.0f; // Standard tile size
            
            // Render all tiles in the map
            for (int y = 0; y < map->getHeight(); y++) {
                for (int x = 0; x < map->getWidth(); x++) {
                    // Calculate tile position
                    glm::vec2 tilePos(x * TILE_SIZE, y * TILE_SIZE);
                    
                    // Draw tile sprite
                    spriteRenderer.drawSprite(
                        TILE_TEXTURE_ID,
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
            // For now, use a placeholder texture ID and size
            // TODO: Load actual player sprite texture
            const uint32_t PLACEHOLDER_TEXTURE_ID = 1;
            const glm::vec2 PLAYER_SIZE(64.0f, 64.0f);
            
            spriteRenderer.drawSprite(
                PLACEHOLDER_TEXTURE_ID,
                player->getPosition(),
                PLAYER_SIZE
            );
        }
    }
    
    // End rendering frame
    spriteRenderer.endFrame();
}

} // namespace d2::rendering