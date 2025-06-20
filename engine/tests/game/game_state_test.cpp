#include <gtest/gtest.h>
#include <memory>
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "game/monster.h"
#include "game/entity_manager.h"

using namespace d2::game;

class GameStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
};

TEST_F(GameStateTest, CreateGameState) {
    GameState gameState;
    
    // GameState should exist and be empty initially
    EXPECT_FALSE(gameState.hasPlayer());
}

TEST_F(GameStateTest, AddPlayerToGameState) {
    GameState gameState;
    
    Character character(CharacterClass::PALADIN);
    auto player = std::make_shared<Player>(character);
    
    gameState.setPlayer(player);
    
    EXPECT_TRUE(gameState.hasPlayer());
    EXPECT_EQ(gameState.getPlayer(), player);
}

// Test for Phase 16: Add monsters to GameState
TEST_F(GameStateTest, AddMonstersToGameState) {
    GameState gameState;
    
    // Should be able to add monsters to the game state
    auto skeleton = std::make_shared<Monster>(MonsterType::SKELETON, 5);
    skeleton->setPosition(100, 200);
    
    auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 7);
    zombie->setPosition(300, 400);
    
    EntityId skeletonId = gameState.addMonster(skeleton);
    EntityId zombieId = gameState.addMonster(zombie);
    
    // Should have valid IDs
    EXPECT_NE(skeletonId, INVALID_ENTITY_ID);
    EXPECT_NE(zombieId, INVALID_ENTITY_ID);
    EXPECT_NE(skeletonId, zombieId);
    
    // Should be able to retrieve monsters
    EXPECT_EQ(gameState.getMonsterCount(), 2);
    
    auto retrievedSkeleton = gameState.getMonster(skeletonId);
    EXPECT_NE(retrievedSkeleton, nullptr);
    EXPECT_EQ(retrievedSkeleton->getType(), MonsterType::SKELETON);
    EXPECT_EQ(retrievedSkeleton->getLevel(), 5);
}

// Test for Phase 16: Get all monsters from GameState
TEST_F(GameStateTest, GetAllMonsters) {
    GameState gameState;
    
    // Add multiple monsters
    auto skeleton1 = std::make_shared<Monster>(MonsterType::SKELETON, 5);
    auto skeleton2 = std::make_shared<Monster>(MonsterType::SKELETON, 6);
    auto zombie = std::make_shared<Monster>(MonsterType::ZOMBIE, 7);
    
    gameState.addMonster(skeleton1);
    gameState.addMonster(skeleton2);
    gameState.addMonster(zombie);
    
    // Get all monsters
    auto allMonsters = gameState.getAllMonsters();
    
    EXPECT_EQ(allMonsters.size(), 3);
    
    // Check that all monsters are in the list
    int skeletonCount = 0;
    int zombieCount = 0;
    
    for (const auto& [id, monster] : allMonsters) {
        EXPECT_NE(id, INVALID_ENTITY_ID);
        EXPECT_NE(monster, nullptr);
        
        if (monster->getType() == MonsterType::SKELETON) {
            skeletonCount++;
        } else if (monster->getType() == MonsterType::ZOMBIE) {
            zombieCount++;
        }
    }
    
    EXPECT_EQ(skeletonCount, 2);
    EXPECT_EQ(zombieCount, 1);
}