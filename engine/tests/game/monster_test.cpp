#include <gtest/gtest.h>
#include "game/monster.h"

using namespace d2::game;

class MonsterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(MonsterTest, CreateBasicMonster) {
    Monster skeleton(MonsterType::SKELETON, 1);
    
    EXPECT_EQ(skeleton.getType(), MonsterType::SKELETON);
    EXPECT_EQ(skeleton.getLevel(), 1);
    EXPECT_GT(skeleton.getLife(), 0);
    EXPECT_GT(skeleton.getDamage(), 0);
    EXPECT_GE(skeleton.getDefense(), 0);
}