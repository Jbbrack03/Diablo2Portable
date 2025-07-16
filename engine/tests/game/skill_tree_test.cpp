#include <gtest/gtest.h>
#include "game/skill_tree.h"
#include "game/character.h"

using namespace d2::game;

class SkillTreeTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SkillTreeTest, CreateSkillTreeForCharacterClass) {
    // Test that we can create a skill tree for a specific character class
    SkillTree sorcTree(CharacterClass::SORCERESS);
    
    EXPECT_EQ(sorcTree.getCharacterClass(), CharacterClass::SORCERESS);
    EXPECT_GT(sorcTree.getSkillTabCount(), 0);  // Should have skill tabs
    EXPECT_GT(sorcTree.getTotalSkillCount(), 0);  // Should have skills
}