#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "game/player.h"
#include "game/character.h"

using namespace d2::game;

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
};

TEST_F(PlayerTest, CreatePlayerWithCharacter) {
    Character character(CharacterClass::BARBARIAN);
    Player player(character);
    
    EXPECT_EQ(player.getCharacterClass(), CharacterClass::BARBARIAN);
}

TEST_F(PlayerTest, PlayerHasPositionInWorld) {
    Character character(CharacterClass::SORCERESS);
    Player player(character);
    
    // Player should start at origin by default
    glm::vec2 position = player.getPosition();
    EXPECT_FLOAT_EQ(position.x, 0.0f);
    EXPECT_FLOAT_EQ(position.y, 0.0f);
}