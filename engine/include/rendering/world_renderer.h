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
    virtual ~WorldRenderer() = default;
    
    virtual void initialize(const d2portable::core::AssetManager& assetManager);
    virtual void render(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer);
    virtual void renderWithCamera(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer, const Camera& camera);
    
    void setHUDEnabled(bool enabled);
    bool isHUDEnabled() const;

protected:
    const d2portable::core::AssetManager* assetManager_ = nullptr;
    bool hudEnabled_ = false;
};

} // namespace d2::rendering