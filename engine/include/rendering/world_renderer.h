#pragma once

#include <unordered_map>
#include "game/entity.h"
#include "rendering/sprite_animation.h"

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
    
    // Animation management
    void setEntityAnimation(d2::game::EntityId entityId, const SpriteAnimation& animation);
    void updateAnimations(float deltaTime);

protected:
    const d2portable::core::AssetManager* assetManager_ = nullptr;
    bool hudEnabled_ = false;
    
    // Entity animations
    std::unordered_map<d2::game::EntityId, SpriteAnimation> entityAnimations_;
};

} // namespace d2::rendering