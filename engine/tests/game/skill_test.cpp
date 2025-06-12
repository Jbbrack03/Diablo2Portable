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