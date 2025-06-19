#include <gtest/gtest.h>
#include <memory>
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

TEST_F(GameStateTest, AddPlayerToGameState) {
    GameState gameState;
    
    Character character(CharacterClass::PALADIN);
    auto player = std::make_shared<Player>(character);
    
    gameState.setPlayer(player);
    
    EXPECT_TRUE(gameState.hasPlayer());
    EXPECT_EQ(gameState.getPlayer(), player);
}