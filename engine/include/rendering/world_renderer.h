#pragma once

namespace d2portable::core {
class AssetManager;
}

namespace d2::rendering {

class SpriteRenderer;

} // namespace d2::rendering

namespace d2::game {

class GameState;

} // namespace d2::game

namespace d2::rendering {

class Camera;

class WorldRenderer {
public:
    WorldRenderer() = default;
    ~WorldRenderer() = default;
    
    void initialize(const d2portable::core::AssetManager& assetManager);
    void render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer);
    void renderWithCamera(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer, const Camera& camera);

private:
    const d2portable::core::AssetManager* assetManager_ = nullptr;
};

} // namespace d2::rendering