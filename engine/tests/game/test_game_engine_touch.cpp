#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "game/game_engine.h"
#include "input/touch_input.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"

namespace d2 {

class GameEngineTouchTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>();
        engine->initialize();
        engine->start(); // Need to start the engine!
        
        // Add a player to the game state
        auto character = game::Character(game::CharacterClass::BARBARIAN);
        auto player = std::make_shared<game::Player>(character);
        engine->getGameState()->setPlayer(player);
    }
    
    std::unique_ptr<GameEngine> engine;
};

TEST_F(GameEngineTouchTest, ProcessTouchInput) {
    // Set screen size first
    engine->setScreenSize(800, 600);
    
    // Get initial player position
    auto player = engine->getGameState()->getPlayer();
    glm::vec2 initialPos = player->getPosition();
    
    // Process touch input in left half of screen (movement area)
    // This should trigger movement to the right
    engine->processTouchInput(300.0f, 300.0f, input::TouchAction::DOWN);
    engine->update(0.016f); // One frame
    
    // Player should have moved
    glm::vec2 newPos = player->getPosition();
    EXPECT_GT(newPos.x, initialPos.x);
}

TEST_F(GameEngineTouchTest, TouchInputWithVirtualJoystick) {
    // Set screen size and enable virtual joystick
    engine->setScreenSize(800, 600);
    engine->setTouchControlMode(GameEngine::TouchControlMode::VIRTUAL_JOYSTICK);
    
    auto player = engine->getGameState()->getPlayer();
    glm::vec2 initialPos = player->getPosition();
    
    // Touch to the right of joystick center for horizontal movement
    // Joystick center is at (150, 450)
    engine->processTouchInput(200.0f, 450.0f, input::TouchAction::DOWN);
    engine->update(0.016f);
    
    // Player should have moved
    EXPECT_NE(player->getPosition(), initialPos);
}

TEST_F(GameEngineTouchTest, TouchTapAction) {
    // Quick tap should trigger action
    engine->processTouchInput(600.0f, 400.0f, input::TouchAction::DOWN);
    engine->processTouchInput(600.0f, 400.0f, input::TouchAction::UP);
    
    // Check if action was triggered (would need to add action tracking)
    EXPECT_TRUE(engine->wasActionTriggered());
}

} // namespace d2