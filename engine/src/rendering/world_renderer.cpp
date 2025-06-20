#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "game/game_state.h"
#include "game/player.h"

namespace d2::rendering {

void WorldRenderer::render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer) {
    // Begin rendering frame
    spriteRenderer.beginFrame();
    
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