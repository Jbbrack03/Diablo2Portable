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