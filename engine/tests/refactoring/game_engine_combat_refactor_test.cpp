#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "game/monster.h"
#include <glm/glm.hpp>

class GameEngineCombatRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<d2::GameEngine>();
        // Initialize with empty path
        engine->initialize("");
        engine->start();
        
        gameState = engine->getGameState();
        ASSERT_NE(gameState, nullptr);
    }
    
    std::unique_ptr<d2::GameEngine> engine;
    d2::game::GameState* gameState;
};

TEST_F(GameEngineCombatRefactorTest, ProcessCombatHandlesNoPlayer) {
    // Should not crash when no player exists
    EXPECT_NO_THROW(engine->processCombat(0.016f));
}

TEST_F(GameEngineCombatRefactorTest, ProcessCombatHandlesNoMonsters) {
    // Create a player
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Should handle case with no monsters gracefully
    EXPECT_NO_THROW(engine->processCombat(0.016f));
}

TEST_F(GameEngineCombatRefactorTest, ProcessCombatDamagesNearbyMonsters) {
    // Create a player at origin
    d2::game::Character character(d2::game::CharacterClass::BARBARIAN);
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition(glm::vec2(0, 0));
    gameState->setPlayer(player);
    
    // Create a nearby monster (within melee range)
    auto nearMonster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::SKELETON, 1);
    nearMonster->setPosition(25, 0); // 25 units away (within 50 unit melee range)
    int nearInitialLife = nearMonster->getCurrentLife();
    gameState->addMonster(nearMonster);
    
    // Create a far monster (outside melee range)
    auto farMonster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::ZOMBIE, 1);
    farMonster->setPosition(100, 0); // 100 units away (outside 50 unit melee range)
    int farInitialLife = farMonster->getCurrentLife();
    gameState->addMonster(farMonster);
    
    // Process combat
    engine->processCombat(0.016f);
    
    // Near monster should be damaged
    EXPECT_LT(nearMonster->getCurrentLife(), nearInitialLife);
    
    // Far monster should not be damaged
    EXPECT_EQ(farMonster->getCurrentLife(), farInitialLife);
}

TEST_F(GameEngineCombatRefactorTest, ProcessCombatSkipsDeadMonsters) {
    // Create a player
    d2::game::Character character(d2::game::CharacterClass::SORCERESS);
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition(glm::vec2(0, 0));
    gameState->setPlayer(player);
    
    // Create a dead monster
    auto deadMonster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::DEMON, 1);
    deadMonster->setPosition(10, 0); // Very close
    // Kill the monster
    while (deadMonster->getCurrentLife() > 0) {
        deadMonster->takeDamage(10);
    }
    gameState->addMonster(deadMonster);
    
    // Process combat should not crash
    EXPECT_NO_THROW(engine->processCombat(0.016f));
}

TEST_F(GameEngineCombatRefactorTest, ProcessCombatCalculatesDistanceCorrectly) {
    // Create a player
    d2::game::Character character(d2::game::CharacterClass::NECROMANCER);
    auto player = std::make_shared<d2::game::Player>(character);
    player->setPosition(glm::vec2(100, 100));
    gameState->setPlayer(player);
    
    // Create monsters at exact melee range boundaries
    auto borderMonster = std::make_shared<d2::game::Monster>(d2::game::MonsterType::FALLEN, 1);
    borderMonster->setPosition(150, 100); // Exactly 50 units away
    int borderInitialLife = borderMonster->getCurrentLife();
    gameState->addMonster(borderMonster);
    
    // Process combat
    engine->processCombat(0.016f);
    
    // Monster at exactly 50 units should be damaged
    EXPECT_LT(borderMonster->getCurrentLife(), borderInitialLife);
}