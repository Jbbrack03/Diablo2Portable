#include <gtest/gtest.h>
#include "game/monster.h"
#include "game/combat_engine.h"
#include "game/entity.h"

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

// Test for Phase 5, Task 5.2: Monster System - Advanced AI behaviors
TEST_F(MonsterTest, AdvancedAIBehaviors) {
    Monster skeleton(MonsterType::SKELETON, 10);
    skeleton.setPosition(0, 0);
    
    // Test PATROLLING state - monster moves in area without target
    skeleton.startPatrolling(50, 50);  // Patrol around position (50,50)
    skeleton.updateAI();
    EXPECT_EQ(skeleton.getAIState(), AIState::PATROLLING);
    
    // Test ATTACKING state - when close to target
    skeleton.setTarget(5, 5);  // Close target
    skeleton.updateAI();
    EXPECT_EQ(skeleton.getAIState(), AIState::ATTACKING);
    
    // Test FLEEING state - when low health
    skeleton.takeDamage(80);  // Reduce health significantly
    skeleton.updateAI();
    EXPECT_EQ(skeleton.getAIState(), AIState::FLEEING);
}

// Test for Phase 5, Task 5.2: Monster System - Group behavior system
TEST_F(MonsterTest, GroupBehaviors) {
    MonsterGroup group;
    
    // Add monsters to the group
    auto skeleton1 = std::make_unique<Monster>(MonsterType::SKELETON, 5);
    auto skeleton2 = std::make_unique<Monster>(MonsterType::SKELETON, 5);
    
    skeleton1->setPosition(100, 100);
    skeleton2->setPosition(105, 105);
    
    int monster1Id = group.addMonster(std::move(skeleton1));
    int monster2Id = group.addMonster(std::move(skeleton2));
    
    // Test group targeting - when one monster spots a target, others should join
    group.setGroupTarget(monster1Id, 200, 200);
    group.updateGroupAI();
    
    // Both monsters should now be seeking the same target
    auto* monster1 = group.getMonster(monster1Id);
    auto* monster2 = group.getMonster(monster2Id);
    
    EXPECT_EQ(monster1->getAIState(), AIState::SEEKING);
    EXPECT_EQ(monster2->getAIState(), AIState::SEEKING);
}

// Test for Phase 5, Task 5.2: Advanced Monster AI - Pack hunting behavior
TEST_F(MonsterTest, PackHuntingBehavior) {
    MonsterSpawner spawner;
    MonsterGroup pack;
    
    // Create a pack of monsters
    auto monster1 = spawner.spawnMonster(MonsterType::FALLEN, 10, 0, 0);
    auto monster2 = spawner.spawnMonster(MonsterType::FALLEN, 10, 1, 0);
    auto monster3 = spawner.spawnMonster(MonsterType::FALLEN, 10, 2, 0);
    
    int id1 = pack.addMonster(std::move(monster1));
    int id2 = pack.addMonster(std::move(monster2));
    int id3 = pack.addMonster(std::move(monster3));
    
    // Set pack target
    pack.setGroupTarget(id1, 100, 100);  // Player position
    
    // Update pack AI - monsters should coordinate
    pack.updateGroupAI();
    
    // All monsters should be seeking the same target
    EXPECT_EQ(pack.getMonster(id1)->getAIState(), AIState::SEEKING);
    EXPECT_EQ(pack.getMonster(id2)->getAIState(), AIState::SEEKING);
    EXPECT_EQ(pack.getMonster(id3)->getAIState(), AIState::SEEKING);
}

// Test for Phase 5, Task 5.2: Advanced Monster AI - Fear behavior
TEST_F(MonsterTest, FearBehaviorWhenOutnumbered) {
    Monster monster(MonsterType::SKELETON, 5);
    monster.setPosition(50, 50);
    
    // Monster should flee when severely outnumbered or low health
    monster.takeDamage(70);  // Reduce to 15 health (from 85)
    monster.updateAI();
    
    // Low health should trigger fleeing behavior
    EXPECT_EQ(monster.getAIState(), AIState::FLEEING);
}

// Test for Phase 5, Task 5.2: Advanced Monster AI - Territorial behavior
TEST_F(MonsterTest, TerritorialBehavior) {
    Monster guardian(MonsterType::GOLEM, 15);
    guardian.setPosition(25, 25);
    guardian.setTerritoryCenter(25, 25, 10);  // 10 unit radius territory
    
    // Set target outside territory - monster should not pursue far
    guardian.setTarget(100, 100);
    guardian.updateAI();
    
    // Should be in SEEKING state but with territorial constraints
    EXPECT_EQ(guardian.getAIState(), AIState::SEEKING);
    EXPECT_TRUE(guardian.hasTerritory());
    EXPECT_EQ(guardian.getTerritoryRadius(), 10);
}

// Test for Phase 5, Task 5.2: Advanced Monster AI - Elite monster abilities
TEST_F(MonsterTest, EliteMonsterAbilities) {
    Monster elite(MonsterType::DEMON, 20);
    elite.setEliteType("Champion");  // Elite monster type
    
    // Elite monsters should have enhanced stats
    EXPECT_GT(elite.getLife(), 200);  // Higher than normal level 20 demon
    EXPECT_TRUE(elite.isElite());
    EXPECT_EQ(elite.getEliteType(), "Champion");
    
    // Elite should have special abilities
    auto abilities = elite.getSpecialAbilities();
    EXPECT_GT(abilities.size(), 0);
}

// Test for Phase 5, Task 5.2: Advanced Monster AI - Sleep/awakening behavior  
TEST_F(MonsterTest, SleepAwakeningBehavior) {
    Monster sleeper(MonsterType::ZOMBIE, 8);
    sleeper.setPosition(10, 10);
    sleeper.setSleeping(true);
    
    // Sleeping monster should be in IDLE state
    EXPECT_EQ(sleeper.getAIState(), AIState::IDLE);
    EXPECT_TRUE(sleeper.isSleeping());
    
    // Should wake up when player gets close
    sleeper.checkPlayerProximity(25, 25, 25.0f);  // Player far enough to trigger SEEKING, not ATTACKING
    sleeper.updateAI();
    
    EXPECT_FALSE(sleeper.isSleeping());
    EXPECT_EQ(sleeper.getAIState(), AIState::SEEKING);
}

// Test for Phase 16: Monster inherits from Entity
TEST_F(MonsterTest, MonsterInheritsFromEntity) {
    Monster skeleton(MonsterType::SKELETON, 1);
    
    // Monster should be an Entity
    Entity* entity = &skeleton;
    EXPECT_NE(entity, nullptr);
    
    // Should be able to use Entity methods
    entity->setPosition(glm::vec2(100.0f, 200.0f));
    EXPECT_EQ(entity->getPosition(), glm::vec2(100.0f, 200.0f));
    
    // Monster's own position methods should also work
    EXPECT_EQ(skeleton.getPositionX(), 100);
    EXPECT_EQ(skeleton.getPositionY(), 200);
}