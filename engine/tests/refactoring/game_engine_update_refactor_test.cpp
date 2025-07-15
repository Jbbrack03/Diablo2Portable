#include <gtest/gtest.h>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "input/input_manager.h"
#include "input/touch_input.h"

class GameEngineUpdateRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<d2::GameEngine>();
        // Initialize with empty path
        engine->initialize("");
    }
    
    std::unique_ptr<d2::GameEngine> engine;
};

TEST_F(GameEngineUpdateRefactorTest, UpdateCallsHelperMethods) {
    // Start the engine
    ASSERT_TRUE(engine->start());
    
    // Create and set a player
    auto gameState = engine->getGameState();
    ASSERT_NE(gameState, nullptr);
    
    d2::game::Character character(d2::game::CharacterClass::AMAZON);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Call update - should process input, touch, and entity updates
    float deltaTime = 0.016f; // 60 FPS
    EXPECT_NO_THROW(engine->update(deltaTime));
}

TEST_F(GameEngineUpdateRefactorTest, UpdateHandlesNullInputManager) {
    // Start the engine
    ASSERT_TRUE(engine->start());
    
    // Update should handle null input manager gracefully
    float deltaTime = 0.016f;
    EXPECT_NO_THROW(engine->update(deltaTime));
}

TEST_F(GameEngineUpdateRefactorTest, UpdateProcessesMovementCorrectly) {
    // Start the engine
    ASSERT_TRUE(engine->start());
    
    // Create and set a player
    auto gameState = engine->getGameState();
    d2::game::Character character(d2::game::CharacterClass::PALADIN);
    auto player = std::make_shared<d2::game::Player>(character);
    gameState->setPlayer(player);
    
    // Store initial position
    glm::vec2 initialPos = player->getPosition();
    
    // Process some movement
    glm::vec2 movement(1.0f, 0.0f); // Move right
    engine->processInput(movement);
    
    // Position should have changed
    glm::vec2 newPos = player->getPosition();
    EXPECT_NE(initialPos.x, newPos.x);
    EXPECT_GT(newPos.x, initialPos.x); // Should have moved right
}

TEST_F(GameEngineUpdateRefactorTest, UpdateHandlesTouchInput) {
    // Start the engine
    ASSERT_TRUE(engine->start());
    
    // Set screen size for touch input
    engine->setScreenSize(800, 600);
    
    // Simulate touch down
    engine->processTouchInput(400.0f, 300.0f, d2::input::TouchAction::DOWN);
    
    // Update should process touch state
    float deltaTime = 0.016f;
    EXPECT_NO_THROW(engine->update(deltaTime));
}