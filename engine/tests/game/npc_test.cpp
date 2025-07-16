#include <gtest/gtest.h>
#include "game/npc.h"
#include <memory>

namespace d2::game {

class NPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};

// Phase 44: Missing Game Systems - NPC Implementation
TEST_F(NPCTest, CreateBasicNPC) {
    NPC npc("Test NPC", NPCType::NEUTRAL);
    
    EXPECT_EQ(npc.getName(), "Test NPC");
    EXPECT_EQ(npc.getType(), NPCType::NEUTRAL);
    EXPECT_EQ(npc.getState(), NPCState::IDLE);
    EXPECT_EQ(npc.getHealth(), 100); // Default health
    EXPECT_EQ(npc.getMaxHealth(), 100);
}

TEST_F(NPCTest, NPCPositionAndMovement) {
    NPC npc("Mobile NPC", NPCType::NEUTRAL);
    
    glm::vec2 initial_pos(100.0f, 200.0f);
    npc.setPosition(initial_pos);
    
    EXPECT_EQ(npc.getPosition(), initial_pos);
}

TEST_F(NPCTest, NPCAnimationSystem) {
    NPC npc("Animated NPC", NPCType::NEUTRAL);
    
    npc.setAnimation("idle");
    EXPECT_EQ(npc.getCurrentAnimation(), "idle");
    
    npc.setAnimation("walk");
    EXPECT_EQ(npc.getCurrentAnimation(), "walk");
}

TEST_F(NPCTest, NPCDialogSystem) {
    NPC npc("Talkative NPC", NPCType::QUEST_GIVER);
    
    EXPECT_EQ(npc.getDialogOptions().size(), 0);
    
    npc.addDialogOption("Hello, traveler!");
    npc.addDialogOption("I have a quest for you.");
    
    EXPECT_EQ(npc.getDialogOptions().size(), 2);
    EXPECT_EQ(npc.getDialogOptions()[0], "Hello, traveler!");
    EXPECT_EQ(npc.getDialogOptions()[1], "I have a quest for you.");
}

TEST_F(NPCTest, VendorFunctionality) {
    Vendor vendor("Blacksmith");
    
    EXPECT_EQ(vendor.getName(), "Blacksmith");
    EXPECT_EQ(vendor.getType(), NPCType::VENDOR);
    EXPECT_TRUE(vendor.canInteract());
    
    // Add items to vendor inventory
    vendor.addItem(1001, 5, 100);  // item_id, quantity, price
    vendor.addItem(1002, 1, 500);
    
    EXPECT_TRUE(vendor.hasItem(1001));
    EXPECT_TRUE(vendor.hasItem(1002));
    EXPECT_FALSE(vendor.hasItem(1003));
    
    EXPECT_EQ(vendor.getItemPrice(1001), 100);
    EXPECT_EQ(vendor.getItemPrice(1002), 500);
}

TEST_F(NPCTest, QuestGiverFunctionality) {
    QuestGiver questGiver("Elder");
    
    EXPECT_EQ(questGiver.getName(), "Elder");
    EXPECT_EQ(questGiver.getType(), NPCType::QUEST_GIVER);
    EXPECT_TRUE(questGiver.canInteract());
    
    // Add quests
    questGiver.addQuest(101);
    questGiver.addQuest(102);
    
    EXPECT_TRUE(questGiver.hasQuest(101));
    EXPECT_TRUE(questGiver.hasQuest(102));
    EXPECT_FALSE(questGiver.hasQuest(103));
    
    // Complete quest
    questGiver.completeQuest(101);
    
    // Quest should still be "has quest" but marked as completed
    EXPECT_TRUE(questGiver.hasQuest(101));
}

TEST_F(NPCTest, NPCHealthSystem) {
    NPC npc("Damageable NPC", NPCType::MONSTER);
    
    EXPECT_EQ(npc.getHealth(), 100);
    EXPECT_EQ(npc.getMaxHealth(), 100);
    
    npc.setHealth(50);
    EXPECT_EQ(npc.getHealth(), 50);
    EXPECT_EQ(npc.getMaxHealth(), 100);
}

TEST_F(NPCTest, NPCUpdate) {
    NPC npc("Updated NPC", NPCType::NEUTRAL);
    
    // Update should not crash
    npc.update(0.016f); // 60 FPS delta
    
    EXPECT_EQ(npc.getState(), NPCState::IDLE);
}

} // namespace d2::game