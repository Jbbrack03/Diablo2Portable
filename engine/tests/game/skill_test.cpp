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