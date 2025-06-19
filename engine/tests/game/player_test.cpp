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