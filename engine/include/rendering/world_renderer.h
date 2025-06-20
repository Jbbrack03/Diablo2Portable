#pragma once

namespace d2::rendering {

class SpriteRenderer;

} // namespace d2::rendering

namespace d2::game {

class GameState;

} // namespace d2::game

namespace d2::rendering {

class WorldRenderer {
public:
    WorldRenderer() = default;
    ~WorldRenderer() = default;
    
    void render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer);
};

} // namespace d2::rendering