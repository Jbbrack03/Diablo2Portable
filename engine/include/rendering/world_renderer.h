#pragma once

#include <unordered_map>
#include <string>
#include "game/entity.h"
#include "game/character.h"
#include "game/monster.h"
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
    
    // Dynamic sprite loading
    bool hasLoadedSprite(const std::string& spriteName) const;
    uint32_t getTextureIdForEntity(d2::game::EntityId entityId) const;
    void cleanupUnusedSprites();
    void removeEntityTexture(d2::game::EntityId entityId);

protected:
    const d2portable::core::AssetManager* assetManager_ = nullptr;
    bool hudEnabled_ = false;
    
    // Entity animations
    std::unordered_map<d2::game::EntityId, SpriteAnimation> entityAnimations_;
    
    // Sprite cache
    std::unordered_map<std::string, uint32_t> spriteCache_;
    std::unordered_map<d2::game::EntityId, uint32_t> entityTextureMap_;
    
    // Helper methods
    std::string getSpriteName(d2::game::CharacterClass charClass) const;
    std::string getMonsterSpriteName(d2::game::MonsterType type) const;
    uint32_t loadOrGetSprite(const std::string& spriteName);
    
    // Refactored rendering methods
    void renderTiles(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer, const Camera* camera = nullptr);
    void renderEntities(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer);
    void renderHUD(const d2::game::GameState& gameState, SpriteRenderer& spriteRenderer);
    uint32_t getTileTextureId() const;
};

} // namespace d2::rendering