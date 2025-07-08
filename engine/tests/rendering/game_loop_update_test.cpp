#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include <glm/glm.hpp>

namespace d2 {

class GameLoopUpdateTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>();
        engine->initialize();
        engine->start();
        
        // Get the game state
        gameState = engine->getGameState();
    }

    void TearDown() override {
        engine->stop();
        engine.reset();
    }

    std::unique_ptr<GameEngine> engine;
    game::GameState* gameState = nullptr;
};

// Test that monsters actually move towards their targets in the game loop
TEST_F(GameLoopUpdateTest, MonstersUpdatePositionTowardsTarget) {
    ASSERT_NE(gameState, nullptr);
    
    // Create a player at origin
    game::Character character(game::CharacterClass::BARBARIAN);
    auto player = std::make_shared<game::Player>(character);
    player->setPosition(glm::vec2(0.0f, 0.0f));
    gameState->setPlayer(player);
    
    // Create a monster some distance away
    auto monster = std::make_shared<game::Monster>(game::MonsterType::ZOMBIE, 1);
    glm::vec2 monsterStartPos(100.0f, 0.0f);
    monster->setPosition(100, 0);  // Monster uses int x, y
    
    // Set target to player's position
    glm::vec2 playerPos = player->getPosition();
    monster->setTarget(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));
    gameState->addMonster(monster);
    
    // Run the update loop - monsters should move towards their target
    float deltaTime = 1.0f; // 1 second
    engine->update(deltaTime);
    
    // Check that the monster has moved towards the player
    glm::vec2 newPos = monster->getPosition();
    float distanceToPlayer = glm::length(newPos - player->getPosition());
    float initialDistance = glm::length(monsterStartPos - player->getPosition());
    
    EXPECT_LT(distanceToPlayer, initialDistance) 
        << "Monster should have moved closer to the player target";
}

} // namespace d2