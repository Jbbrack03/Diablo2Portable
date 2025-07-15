#include <gtest/gtest.h>
#include <memory>
#include "rendering/world_renderer.h"
#include "rendering/sprite_renderer.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include "game/character.h"
#include "core/asset_manager.h"
#include "rendering/texture_manager.h"

class DynamicSpriteLoadingTest : public ::testing::Test {
protected:
    void SetUp() override {
        assetManager_ = std::make_shared<d2portable::core::AssetManager>();
        assetManager_->initialize("test_assets");
        
        textureManager_ = std::make_shared<d2::rendering::TextureManager>();
        renderer_ = std::make_unique<d2::rendering::WorldRenderer>();
        renderer_->initialize(*assetManager_);
    }
    
    std::shared_ptr<d2portable::core::AssetManager> assetManager_;
    std::shared_ptr<d2::rendering::TextureManager> textureManager_;
    std::unique_ptr<d2::rendering::WorldRenderer> renderer_;
};

TEST_F(DynamicSpriteLoadingTest, LoadsSpritesFromAssetManager) {
    // Create game state with a player
    d2::game::GameState gameState;
    
    // Create a character and player
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState.setPlayer(player);
    
    // Create sprite renderer
    d2::rendering::SpriteRenderer spriteRenderer;
    
    // Render the game state
    renderer_->render(gameState, spriteRenderer);
    
    // Should have loaded and cached sprite for the player
    EXPECT_TRUE(renderer_->hasLoadedSprite("barbarian_walk"));
    EXPECT_GT(renderer_->getTextureIdForEntity(player->getId()), 0);
}

TEST_F(DynamicSpriteLoadingTest, CachesLoadedSprites) {
    d2::game::GameState gameState;
    
    // Create two monsters of the same type
    auto monster1 = std::make_shared<d2::game::Monster>(d2::game::MonsterType::ZOMBIE, 1);
    monster1->setPosition(100, 100);
    auto monster2 = std::make_shared<d2::game::Monster>(d2::game::MonsterType::ZOMBIE, 1);
    monster2->setPosition(200, 200);
    
    auto id1 = gameState.addMonster(monster1);
    auto id2 = gameState.addMonster(monster2);
    
    d2::rendering::SpriteRenderer spriteRenderer;
    
    // Render both monsters
    renderer_->render(gameState, spriteRenderer);
    
    auto textureId1 = renderer_->getTextureIdForEntity(id1);
    auto textureId2 = renderer_->getTextureIdForEntity(id2);
    
    // Same type should use same texture (cached)
    EXPECT_EQ(textureId1, textureId2);
    EXPECT_GT(textureId1, 0); // Should have valid texture ID
}

TEST_F(DynamicSpriteLoadingTest, LoadsDifferentSpritesForDifferentEntities) {
    d2::game::GameState gameState;
    
    // Create different entity types
    d2::game::Character character(d2::game::CharacterClass::AMAZON);
    auto player = std::make_shared<d2::game::Player>(character);
    auto monster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::SKELETON, 1);
    monster->setPosition(100, 100);
    
    gameState.setPlayer(player);
    auto monsterEntityId = gameState.addMonster(monster);
    
    d2::rendering::SpriteRenderer spriteRenderer;
    renderer_->render(gameState, spriteRenderer);
    
    auto playerTexture = renderer_->getTextureIdForEntity(player->getId());
    auto monsterTexture = renderer_->getTextureIdForEntity(monsterEntityId);
    
    // Different entities should have different textures
    EXPECT_NE(playerTexture, monsterTexture);
    EXPECT_GT(playerTexture, 0);
    EXPECT_GT(monsterTexture, 0);
}

TEST_F(DynamicSpriteLoadingTest, HandlesAnimationStates) {
    d2::game::GameState gameState;
    
    d2::game::Character character(d2::game::CharacterClass::SORCERESS);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState.setPlayer(player);
    
    d2::rendering::SpriteRenderer spriteRenderer;
    
    // First render - should load sprite
    renderer_->render(gameState, spriteRenderer);
    auto textureId = renderer_->getTextureIdForEntity(player->getId());
    EXPECT_GT(textureId, 0);
    
    // Move player and render again
    player->move(glm::vec2(10.0f, 0.0f));
    renderer_->render(gameState, spriteRenderer);
    
    // Should still use same texture (from cache)
    auto textureIdAfterMove = renderer_->getTextureIdForEntity(player->getId());
    EXPECT_EQ(textureId, textureIdAfterMove);
}

TEST_F(DynamicSpriteLoadingTest, CleanupUnusedSprites) {
    d2::game::GameState gameState;
    
    // Add a monster
    auto monster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::ZOMBIE, 1);
    monster->setPosition(100, 100);
    auto monsterId = gameState.addMonster(monster);
    
    d2::rendering::SpriteRenderer spriteRenderer;
    renderer_->render(gameState, spriteRenderer);
    
    EXPECT_TRUE(renderer_->hasLoadedSprite("zombie_walk"));
    
    // Remove the monster entity from the renderer's tracking
    renderer_->removeEntityTexture(monsterId);
    
    // Cleanup unused sprites
    renderer_->cleanupUnusedSprites();
    
    // Sprite should be removed if no other entities use it
    EXPECT_FALSE(renderer_->hasLoadedSprite("zombie_walk"));
}