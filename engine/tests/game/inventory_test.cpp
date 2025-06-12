#include <gtest/gtest.h>
#include <memory>
#include "game/inventory.h"
#include "game/item.h"
#include "game/character_inventory.h"

using namespace d2::game;

class InventoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.5: Inventory System - Basic container
TEST_F(InventoryTest, CreateBasicInventory) {
    // Create inventory with 10x4 grid (standard Diablo II inventory size)
    Inventory inventory(10, 4);
    
    EXPECT_EQ(inventory.getWidth(), 10);
    EXPECT_EQ(inventory.getHeight(), 4);
    EXPECT_EQ(inventory.getTotalSlots(), 40);
    EXPECT_EQ(inventory.getUsedSlots(), 0);
    EXPECT_TRUE(inventory.isEmpty());
}

// Test for Phase 4, Task 4.5: Inventory System - Add items
TEST_F(InventoryTest, AddItemsToInventory) {
    Inventory inventory(10, 4);
    
    // Create a small item (1x1)
    auto potion = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion->setSize(1, 1);
    
    // Add item to specific position
    EXPECT_TRUE(inventory.addItem(potion, 0, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 1);
    EXPECT_FALSE(inventory.isEmpty());
    
    // Check item is at correct position
    EXPECT_EQ(inventory.getItemAt(0, 0), potion);
    
    // Create a larger item (2x3 - typical weapon size)
    auto sword = std::make_shared<Item>("Long Sword", ItemType::WEAPON);
    sword->setSize(2, 3);
    
    // Add to different position
    EXPECT_TRUE(inventory.addItem(sword, 2, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 7);  // 1 + (2*3)
    
    // Verify the sword occupies all its slots
    for (int x = 2; x < 4; x++) {
        for (int y = 0; y < 3; y++) {
            EXPECT_EQ(inventory.getItemAt(x, y), sword);
        }
    }
}

// Test for Phase 4, Task 4.5: Inventory System - Remove items
TEST_F(InventoryTest, RemoveItemsFromInventory) {
    Inventory inventory(10, 4);
    
    // Add some items first
    auto potion = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion->setSize(1, 1);
    inventory.addItem(potion, 0, 0);
    
    auto sword = std::make_shared<Item>("Long Sword", ItemType::WEAPON);
    sword->setSize(2, 3);
    inventory.addItem(sword, 2, 0);
    
    EXPECT_EQ(inventory.getUsedSlots(), 7);
    
    // Remove the potion
    EXPECT_TRUE(inventory.removeItem(0, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 6);  // Only sword remains
    EXPECT_EQ(inventory.getItemAt(0, 0), nullptr);
    
    // Remove the sword (can remove from any of its occupied positions)
    EXPECT_TRUE(inventory.removeItem(3, 1));  // Remove from middle of sword
    EXPECT_EQ(inventory.getUsedSlots(), 0);
    
    // Verify all sword slots are now empty
    for (int x = 2; x < 4; x++) {
        for (int y = 0; y < 3; y++) {
            EXPECT_EQ(inventory.getItemAt(x, y), nullptr);
        }
    }
    
    // Try to remove from empty slot
    EXPECT_FALSE(inventory.removeItem(0, 0));
}

// Test for Phase 4, Task 4.5: Inventory System - Capacity limits
TEST_F(InventoryTest, InventoryCapacityLimits) {
    Inventory inventory(4, 2);  // Small 4x2 inventory
    
    // Create a 2x2 item
    auto shield = std::make_shared<Item>("Small Shield", ItemType::ARMOR);
    shield->setSize(2, 2);
    
    // Add first shield
    EXPECT_TRUE(inventory.addItem(shield, 0, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 4);
    
    // Try to add overlapping item
    auto helmet = std::make_shared<Item>("Helmet", ItemType::ARMOR);
    helmet->setSize(2, 2);
    EXPECT_FALSE(inventory.addItem(helmet, 1, 0));  // Overlaps with shield
    
    // Add second shield in remaining space
    auto shield2 = std::make_shared<Item>("Large Shield", ItemType::ARMOR);
    shield2->setSize(2, 2);
    EXPECT_TRUE(inventory.addItem(shield2, 2, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 8);
    
    // Inventory is now full
    EXPECT_TRUE(inventory.isFull());
    
    // Try to add item when full
    auto potion = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion->setSize(1, 1);
    EXPECT_FALSE(inventory.addItem(potion, 0, 0));
    
    // Try to add item that goes out of bounds
    auto sword = std::make_shared<Item>("Long Sword", ItemType::WEAPON);
    sword->setSize(2, 3);
    EXPECT_FALSE(inventory.addItem(sword, 0, 0));  // Too tall for 2-height inventory
    
    // Find space for specific sized item
    inventory.removeItem(0, 0);  // Remove first shield
    EXPECT_FALSE(inventory.isFull());
    
    EXPECT_TRUE(inventory.hasSpaceFor(2, 2));  // Can fit 2x2 item
    EXPECT_FALSE(inventory.hasSpaceFor(3, 2)); // Cannot fit 3x2 item
}

// Test for Phase 4, Task 4.5: Inventory System - Equipment slots
TEST_F(InventoryTest, EquipmentSlots) {
    // Create a character inventory with equipment slots
    CharacterInventory charInv;
    
    // Create equipment items
    auto helmet = std::make_shared<Item>("Iron Helmet", ItemType::ARMOR);
    helmet->setSize(2, 2);
    helmet->setEquipmentSlot(EquipmentSlot::HEAD);
    
    auto sword = std::make_shared<Item>("Broad Sword", ItemType::WEAPON);
    sword->setSize(2, 3);
    sword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    
    auto boots = std::make_shared<Item>("Leather Boots", ItemType::ARMOR);
    boots->setSize(2, 2);
    boots->setEquipmentSlot(EquipmentSlot::FEET);
    
    // Equip items
    EXPECT_TRUE(charInv.equipItem(helmet));
    EXPECT_TRUE(charInv.equipItem(sword));
    EXPECT_TRUE(charInv.equipItem(boots));
    
    // Check equipped items
    EXPECT_EQ(charInv.getEquippedItem(EquipmentSlot::HEAD), helmet);
    EXPECT_EQ(charInv.getEquippedItem(EquipmentSlot::MAIN_HAND), sword);
    EXPECT_EQ(charInv.getEquippedItem(EquipmentSlot::FEET), boots);
    
    // Try to equip another helmet (should replace)
    auto betterHelmet = std::make_shared<Item>("Golden Helmet", ItemType::ARMOR);
    betterHelmet->setSize(2, 2);
    betterHelmet->setEquipmentSlot(EquipmentSlot::HEAD);
    
    EXPECT_TRUE(charInv.equipItem(betterHelmet));
    EXPECT_EQ(charInv.getEquippedItem(EquipmentSlot::HEAD), betterHelmet);
    
    // Previous helmet should be in backpack
    EXPECT_TRUE(charInv.getBackpack().contains(helmet));
    
    // Unequip item
    EXPECT_TRUE(charInv.unequipItem(EquipmentSlot::MAIN_HAND));
    EXPECT_EQ(charInv.getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
    EXPECT_TRUE(charInv.getBackpack().contains(sword));
}