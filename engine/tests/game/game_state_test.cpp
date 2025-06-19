#include <gtest/gtest.h>
#include "game/game_state.h"
#include "game/player.h"
#include "game/character.h"

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