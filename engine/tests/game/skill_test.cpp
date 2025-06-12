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

TEST_F(SkillTest, SkillDamageCalculation) {
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    Skill fireBall(SkillType::FIRE, "Fire Ball");
    
    // Set base damage for skills
    fireBolt.setBaseDamage(10, 15);      // 10-15 base damage
    fireBall.setBaseDamage(30, 50);      // 30-50 base damage
    
    // Set damage scaling per level
    fireBolt.setDamagePerLevel(2, 3);    // +2-3 damage per level
    fireBall.setDamagePerLevel(5, 8);    // +5-8 damage per level
    
    // At level 0, no damage
    EXPECT_EQ(fireBolt.getMinDamage(), 0);
    EXPECT_EQ(fireBolt.getMaxDamage(), 0);
    
    // Level up Fire Bolt to 1
    fireBolt.addSkillPoint();
    EXPECT_EQ(fireBolt.getMinDamage(), 10);  // Base damage
    EXPECT_EQ(fireBolt.getMaxDamage(), 15);
    
    // Level up Fire Bolt to 10
    for (int i = 1; i < 10; i++) {
        fireBolt.addSkillPoint();
    }
    EXPECT_EQ(fireBolt.getMinDamage(), 28);  // 10 + 9*2
    EXPECT_EQ(fireBolt.getMaxDamage(), 42);  // 15 + 9*3
    
    // Test with synergies
    fireBall.addSynergy(&fireBolt, 0.10f);  // 10% bonus per Fire Bolt level
    
    // Level Fire Ball to 5
    for (int i = 0; i < 5; i++) {
        fireBall.addSkillPoint();
    }
    
    // Base: 30-50, Level bonus: +20-32 (4 * 5-8), Synergy: 100% bonus (10 levels * 10%)
    // Total: (30+20) * 2.0 = 100, (50+32) * 2.0 = 164
    EXPECT_EQ(fireBall.getMinDamage(), 100);
    EXPECT_EQ(fireBall.getMaxDamage(), 164);
}

TEST_F(SkillTest, SkillManaCostSystem) {
    Skill fireBolt(SkillType::FIRE, "Fire Bolt");
    Skill teleport(SkillType::MAGIC, "Teleport");
    
    // Set base mana costs
    fireBolt.setBaseManaCost(2.5f);    // 2.5 mana base
    teleport.setBaseManaCost(24.0f);   // 24 mana base (high cost skill)
    
    // Set mana cost reduction per level
    fireBolt.setManaCostReduction(0.125f);  // -0.125 per level
    teleport.setManaCostReduction(0.0f);    // No reduction (always 24)
    
    // At level 0, no mana cost
    EXPECT_EQ(fireBolt.getManaCost(), 0);
    EXPECT_EQ(teleport.getManaCost(), 0);
    
    // Level up Fire Bolt to 1
    fireBolt.addSkillPoint();
    EXPECT_FLOAT_EQ(fireBolt.getManaCost(), 2.5f);
    
    // Level up Fire Bolt to 10
    for (int i = 1; i < 10; i++) {
        fireBolt.addSkillPoint();
    }
    // 2.5 - (9 * 0.125) = 2.5 - 1.125 = 1.375
    EXPECT_FLOAT_EQ(fireBolt.getManaCost(), 1.375f);
    
    // Level up Fire Bolt to 20 (max)
    for (int i = 10; i < 20; i++) {
        fireBolt.addSkillPoint();
    }
    // 2.5 - (19 * 0.125) = 2.5 - 2.375 = 0.125
    EXPECT_FLOAT_EQ(fireBolt.getManaCost(), 0.125f);
    
    // Test minimum mana cost (can't go below 0)
    Skill nova(SkillType::LIGHTNING, "Nova");
    nova.setBaseManaCost(5.0f);
    nova.setManaCostReduction(1.0f);  // High reduction
    
    for (int i = 0; i < 20; i++) {
        nova.addSkillPoint();
    }
    // Should be 0, not negative
    EXPECT_FLOAT_EQ(nova.getManaCost(), 0.0f);
    
    // Test Teleport (no reduction)
    for (int i = 0; i < 20; i++) {
        teleport.addSkillPoint();
    }
    EXPECT_FLOAT_EQ(teleport.getManaCost(), 24.0f);
}