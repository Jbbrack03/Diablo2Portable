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

TEST_F(SkillTreeTest, SorceressSkillTreeStructure) {
    // Test that sorceress has the correct skill tree structure
    SkillTree sorcTree(CharacterClass::SORCERESS);
    
    EXPECT_EQ(sorcTree.getSkillTabCount(), 3);  // Fire, Cold, Lightning
    EXPECT_GT(sorcTree.getTotalSkillCount(), 20);  // Should have many skills
    
    // Test individual skill tabs
    SkillTab* fireTab = sorcTree.getSkillTab(0);
    ASSERT_NE(fireTab, nullptr);
    EXPECT_EQ(fireTab->getName(), "Fire Spells");
    EXPECT_GT(fireTab->getSkillCount(), 5);  // Should have multiple fire skills
    
    SkillTab* coldTab = sorcTree.getSkillTab(1);
    ASSERT_NE(coldTab, nullptr);
    EXPECT_EQ(coldTab->getName(), "Cold Spells");
    EXPECT_GT(coldTab->getSkillCount(), 5);  // Should have multiple cold skills
    
    SkillTab* lightningTab = sorcTree.getSkillTab(2);
    ASSERT_NE(lightningTab, nullptr);
    EXPECT_EQ(lightningTab->getName(), "Lightning Spells");
    EXPECT_GT(lightningTab->getSkillCount(), 5);  // Should have multiple lightning skills
}

TEST_F(SkillTreeTest, FindSkillsByName) {
    // Test skill lookup functionality
    SkillTree sorcTree(CharacterClass::SORCERESS);
    
    // Test finding known skills
    Skill* fireBolt = sorcTree.findSkill("Fire Bolt");
    ASSERT_NE(fireBolt, nullptr);
    EXPECT_EQ(fireBolt->getName(), "Fire Bolt");
    EXPECT_EQ(fireBolt->getType(), SkillType::FIRE);
    
    Skill* teleport = sorcTree.findSkill("Teleport");
    ASSERT_NE(teleport, nullptr);
    EXPECT_EQ(teleport->getName(), "Teleport");
    EXPECT_EQ(teleport->getType(), SkillType::MAGIC);
    
    Skill* iceBolt = sorcTree.findSkill("Ice Bolt");
    ASSERT_NE(iceBolt, nullptr);
    EXPECT_EQ(iceBolt->getName(), "Ice Bolt");
    EXPECT_EQ(iceBolt->getType(), SkillType::COLD);
    
    // Test finding non-existent skill
    Skill* nonExistent = sorcTree.findSkill("Non-Existent Skill");
    EXPECT_EQ(nonExistent, nullptr);
}

TEST_F(SkillTreeTest, CharacterSkillTreeIntegration) {
    // Test that characters can access their skill tree
    Character sorceress(CharacterClass::SORCERESS);
    
    // Character should have access to their skill tree
    SkillTree* skillTree = sorceress.getSkillTree();
    ASSERT_NE(skillTree, nullptr);
    EXPECT_EQ(skillTree->getCharacterClass(), CharacterClass::SORCERESS);
    
    // Should be able to find specific skills through character
    Skill* fireBolt = sorceress.findSkill("Fire Bolt");
    ASSERT_NE(fireBolt, nullptr);
    EXPECT_EQ(fireBolt->getName(), "Fire Bolt");
    
    // Test skill point allocation through character
    sorceress.setSkillPoints(5);
    EXPECT_EQ(sorceress.getSkillPoints(), 5);
    
    // Should be able to add skill points to a skill
    EXPECT_TRUE(sorceress.addSkillPoint("Fire Bolt"));
    EXPECT_EQ(fireBolt->getLevel(), 1);
    EXPECT_EQ(sorceress.getSkillPoints(), 4);  // One point used
}