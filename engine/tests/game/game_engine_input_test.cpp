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