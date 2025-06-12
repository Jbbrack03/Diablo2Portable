#include <gtest/gtest.h>
#include "game/monster.h"
#include "game/combat_engine.h"

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

TEST_F(MonsterTest, MonsterLevelScaling) {
    Monster level1Skeleton(MonsterType::SKELETON, 1);
    Monster level10Skeleton(MonsterType::SKELETON, 10);
    
    // Higher level monsters should have more life and damage
    EXPECT_GT(level10Skeleton.getLife(), level1Skeleton.getLife());
    EXPECT_GT(level10Skeleton.getDamage(), level1Skeleton.getDamage());
    EXPECT_GE(level10Skeleton.getDefense(), level1Skeleton.getDefense());
    
    // Specific scaling expectations for level 10 skeleton
    EXPECT_EQ(level10Skeleton.getLife(), 85);    // Base + level scaling
    EXPECT_EQ(level10Skeleton.getDamage(), 15);  // Base + level scaling
}

TEST_F(MonsterTest, BasicAIBehavior) {
    Monster skeleton(MonsterType::SKELETON, 1);
    
    // Monster should start in IDLE state
    EXPECT_EQ(skeleton.getAIState(), AIState::IDLE);
    
    // Monster should be able to transition to SEEKING when target is detected
    skeleton.setTarget(100, 200); // Set target position
    skeleton.updateAI();
    EXPECT_EQ(skeleton.getAIState(), AIState::SEEKING);
    
    // Monster should clear target and return to IDLE
    skeleton.clearTarget();
    skeleton.updateAI();
    EXPECT_EQ(skeleton.getAIState(), AIState::IDLE);
}

TEST_F(MonsterTest, CombatIntegration) {
    Monster skeleton(MonsterType::SKELETON, 10);
    CombatEngine combat;
    
    // Monster should have combat stats that integrate with CombatEngine
    EXPECT_GT(skeleton.getAttackRating(), 0);
    EXPECT_GT(skeleton.getDefense(), 0);
    
    // Test combat calculations with monster stats
    float hitChance = combat.calculateHitChance(
        skeleton.getAttackRating(),
        100,  // Target defense
        skeleton.getLevel(),
        10    // Target level
    );
    
    EXPECT_GT(hitChance, 0.0f);
    EXPECT_LE(hitChance, 1.0f);
    
    // Test damage calculation with monster damage
    int damage = combat.calculateDamage(
        skeleton.getDamage() - 2,  // Min damage (slightly less than max)
        skeleton.getDamage(),      // Max damage
        0                          // No elemental damage
    );
    
    EXPECT_GE(damage, skeleton.getDamage() - 2);
    EXPECT_LE(damage, skeleton.getDamage());
}

// Test for Phase 5, Task 5.2: Monster System - Monster spawning functionality
TEST_F(MonsterTest, MonsterSpawning) {
    MonsterSpawner spawner;
    
    // Test spawning a monster at a specific location
    auto monster = spawner.spawnMonster(MonsterType::SKELETON, 5, 100, 200);
    
    EXPECT_NE(monster, nullptr);
    EXPECT_EQ(monster->getType(), MonsterType::SKELETON);
    EXPECT_EQ(monster->getLevel(), 5);
    EXPECT_EQ(monster->getPositionX(), 100);
    EXPECT_EQ(monster->getPositionY(), 200);
}