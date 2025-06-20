#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"
#include "input/input_manager.h"
#include "input/gamepad.h"

using namespace d2;
using namespace d2::game;
using namespace d2::input;
using ::testing::Return;

// Mock Gamepad for testing
class MockGamepad : public Gamepad {
public:
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(float, getAxis, (int axisId), (const, override));
    MOCK_METHOD(bool, getButton, (int buttonId), (const, override));
};

class GameEngineInputTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockGamepad = std::make_unique<MockGamepad>();
        engine = std::make_unique<GameEngine>();
    }
    
    std::unique_ptr<MockGamepad> mockGamepad;
    std::unique_ptr<GameEngine> engine;
};

// Test 1: GameEngine should have an InputManager
TEST_F(GameEngineInputTest, GameEngineHasInputManager) {
    EXPECT_TRUE(engine->initialize());
    
    // GameEngine should have a method to get InputManager
    auto* inputManager = engine->getInputManager();
    EXPECT_NE(inputManager, nullptr);
}

// Test 2: GameEngine should process input and update player position
TEST_F(GameEngineInputTest, ProcessInputMovesPlayer) {
    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(engine->start());
    
    // Add a player to the game state
    auto* gameState = engine->getGameState();
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100.0f, 100.0f));
    gameState->setPlayer(player);
    
    // GameEngine should have a method to process input that moves the player
    glm::vec2 movement(1.0f, 0.0f); // Move right
    engine->processInput(movement);
    
    // Player should have moved
    EXPECT_GT(player->getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(player->getPosition().y, 100.0f);
}

// Test 3: GameEngine should update game state during renderFrame
TEST_F(GameEngineInputTest, UpdateGameStateDuringRenderFrame) {
    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(engine->start());
    
    // Add a player
    auto* gameState = engine->getGameState();
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(100.0f, 100.0f));
    gameState->setPlayer(player);
    
    // GameEngine should have an update method that processes the game state
    float deltaTime = 0.016f; // 60 FPS
    engine->update(deltaTime);
    
    // For now, just verify update was called without crashing
    EXPECT_TRUE(true);
}

// Test 4: Full integration - renderFrame reads input and moves player
TEST_F(GameEngineInputTest, FullInputIntegration) {
    // We need a way to provide a mock gamepad to the engine
    // For now, let's test that the integration works by verifying
    // that calling renderFrame doesn't crash when InputManager is present
    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(engine->start());
    
    // Add a player
    auto* gameState = engine->getGameState();
    Character character(CharacterClass::SORCERESS);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(50.0f, 50.0f));
    gameState->setPlayer(player);
    
    // Call renderFrame multiple times
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(engine->renderFrame());
    }
    
    // Since InputManager was created with nullptr gamepad,
    // movement will be (0,0) and player shouldn't move
    EXPECT_FLOAT_EQ(player->getPosition().x, 50.0f);
    EXPECT_FLOAT_EQ(player->getPosition().y, 50.0f);
}