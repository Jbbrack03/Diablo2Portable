#include <gtest/gtest.h>
#include <memory>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"

using namespace d2;
using namespace d2::game;

class GameEngineRenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>();
    }
    
    std::unique_ptr<GameEngine> engine;
};

// Test 1: GameEngine should render world with player
TEST_F(GameEngineRenderTest, RenderWorldWithPlayer) {
    EXPECT_TRUE(engine->initialize());
    EXPECT_TRUE(engine->start());
    
    // Add a player to the game state
    auto* gameState = engine->getGameState();
    Character character(CharacterClass::PALADIN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(200.0f, 200.0f));
    gameState->setPlayer(player);
    
    // GameEngine should have a method to get if rendering occurred
    // For now, we'll just verify renderFrame doesn't crash and returns true
    EXPECT_TRUE(engine->renderFrame());
    
    // In a real test, we might check:
    // - That the WorldRenderer was used
    // - That the player was rendered at the correct position
    // - That the camera followed the player
    // But for now, we just verify the basic integration works
}