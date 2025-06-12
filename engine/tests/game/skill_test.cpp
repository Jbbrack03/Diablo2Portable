#include <gtest/gtest.h>
#include "game/skill.h"

using namespace d2::game;

class SkillTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SkillTest, CreateBasicSkillWithNameAndType) {
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    
    EXPECT_EQ(fireBolt.getName(), "Fire Bolt");
    EXPECT_EQ(fireBolt.getType(), SkillType::FIRE);
    EXPECT_EQ(fireBolt.getLevel(), 0);  // Unlearned skill starts at level 0
}

TEST_F(SkillTest, SkillLevelingSystem) {
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    
    // Skill starts at level 0
    EXPECT_EQ(fireBolt.getLevel(), 0);
    EXPECT_EQ(fireBolt.getMaxLevel(), 20);  // Most skills cap at 20
    
    // Level up the skill
    EXPECT_TRUE(fireBolt.addSkillPoint());
    EXPECT_EQ(fireBolt.getLevel(), 1);
    
    // Add more points
    EXPECT_TRUE(fireBolt.addSkillPoint());
    EXPECT_TRUE(fireBolt.addSkillPoint());
    EXPECT_EQ(fireBolt.getLevel(), 3);
    
    // Try to level beyond max - should fail
    for (int i = 3; i < 20; i++) {
        EXPECT_TRUE(fireBolt.addSkillPoint());
    }
    EXPECT_EQ(fireBolt.getLevel(), 20);
    EXPECT_FALSE(fireBolt.addSkillPoint());  // Can't go beyond max
    EXPECT_EQ(fireBolt.getLevel(), 20);
}

TEST_F(SkillTest, SkillPrerequisites) {
    // Create skill tree: Fire Bolt -> Fire Ball -> Meteor
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    Skill fireBall(SkillType::FIRE, "Fire Ball");
    Skill meteor(SkillType::FIRE, "Meteor");
    
    // Set prerequisites
    fireBall.setPrerequisite(&fireBolt, 1);  // Requires Fire Bolt level 1
    meteor.setPrerequisite(&fireBall, 5);    // Requires Fire Ball level 5
    
    // Can't level Fire Ball without Fire Bolt
    EXPECT_FALSE(fireBall.canAddSkillPoint());
    EXPECT_FALSE(fireBall.addSkillPoint());
    EXPECT_EQ(fireBall.getLevel(), 0);
    
    // Level Fire Bolt to meet prerequisite
    EXPECT_TRUE(fireBolt.addSkillPoint());
    
    // Now can level Fire Ball
    EXPECT_TRUE(fireBall.canAddSkillPoint());
    EXPECT_TRUE(fireBall.addSkillPoint());
    EXPECT_EQ(fireBall.getLevel(), 1);
    
    // Can't level Meteor until Fire Ball is level 5
    EXPECT_FALSE(meteor.canAddSkillPoint());
    
    // Level Fire Ball to 5
    for (int i = 1; i < 5; i++) {
        EXPECT_TRUE(fireBall.addSkillPoint());
    }
    EXPECT_EQ(fireBall.getLevel(), 5);
    
    // Now can level Meteor
    EXPECT_TRUE(meteor.canAddSkillPoint());
    EXPECT_TRUE(meteor.addSkillPoint());
    EXPECT_EQ(meteor.getLevel(), 1);
}

TEST_F(SkillTest, SkillSynergies) {
    // Create synergy chain: Fire Bolt boosts Fire Ball, Fire Ball boosts Meteor
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    Skill fireBall(SkillType::FIRE, "Fire Ball");
    Skill meteor(SkillType::FIRE, "Meteor");
    
    // Set synergies - each point in synergy skill adds bonus
    fireBall.addSynergy(&fireBolt, 0.15f);  // 15% damage per Fire Bolt level
    meteor.addSynergy(&fireBall, 0.20f);    // 20% damage per Fire Ball level
    meteor.addSynergy(&fireBolt, 0.10f);    // 10% damage per Fire Bolt level
    
    // No synergy bonus when skills are unleveled
    EXPECT_FLOAT_EQ(fireBall.getSynergyBonus(), 0.0f);
    EXPECT_FLOAT_EQ(meteor.getSynergyBonus(), 0.0f);
    
    // Level Fire Bolt to 5
    for (int i = 0; i < 5; i++) {
        fireBolt.addSkillPoint();
    }
    
    // Fire Ball gets 75% bonus (5 * 15%)
    EXPECT_FLOAT_EQ(fireBall.getSynergyBonus(), 0.75f);
    
    // Level Fire Ball to 10
    for (int i = 0; i < 10; i++) {
        fireBall.addSkillPoint();
    }
    
    // Meteor gets 2.5x bonus (10 * 20% + 5 * 10%)
    EXPECT_FLOAT_EQ(meteor.getSynergyBonus(), 2.5f);
    
    // Test max synergies (3 synergy limit)
    Skill warmth(SkillType::FIRE, "Warmth");
    Skill enchant(SkillType::FIRE, "Enchant");
    
    EXPECT_TRUE(meteor.addSynergy(&warmth, 0.05f));
    EXPECT_FALSE(meteor.addSynergy(&enchant, 0.05f));  // Already has 3 synergies
}