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