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

// Test for Phase 4 Enhancement: Equipment slot validation
TEST_F(InventoryTest, EquipmentSlotValidation) {
    CharacterInventory charInventory;
    
    // Create items with specific slot requirements
    auto helmet = std::make_shared<Item>("Steel Helm", ItemType::ARMOR);
    helmet->setEquipmentSlot(EquipmentSlot::HEAD);
    helmet->setSize(2, 2);
    
    auto sword = std::make_shared<Item>("Long Sword", ItemType::WEAPON);
    sword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    sword->setSize(1, 3);
    
    auto boots = std::make_shared<Item>("Leather Boots", ItemType::ARMOR);
    boots->setEquipmentSlot(EquipmentSlot::FEET);
    boots->setSize(2, 2);
    
    // Test that slot validation is enforced
    EXPECT_TRUE(charInventory.equipItemWithValidation(helmet));
    EXPECT_EQ(charInventory.getEquippedItem(EquipmentSlot::HEAD), helmet);
    
    // Test equipping another item to same slot replaces it
    auto betterHelmet = std::make_shared<Item>("Golden Helm", ItemType::ARMOR);
    betterHelmet->setEquipmentSlot(EquipmentSlot::HEAD);
    betterHelmet->setSize(2, 2);
    
    EXPECT_TRUE(charInventory.equipItemWithValidation(betterHelmet));
    EXPECT_EQ(charInventory.getEquippedItem(EquipmentSlot::HEAD), betterHelmet);
    
    // Test that wrong slot items are rejected
    auto wrongSlotItem = std::make_shared<Item>("Wrong Item", ItemType::ARMOR);
    wrongSlotItem->setEquipmentSlot(EquipmentSlot::HEAD);
    // Try to force it into wrong slot
    EXPECT_FALSE(charInventory.forceEquipToSlot(wrongSlotItem, EquipmentSlot::FEET));
    
    // Test two-handed weapons
    auto twoHandedSword = std::make_shared<Item>("Claymore", ItemType::WEAPON);
    twoHandedSword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    twoHandedSword->setSize(2, 4);
    twoHandedSword->setTwoHanded(true);
    
    EXPECT_TRUE(charInventory.equipItemWithValidation(twoHandedSword));
    EXPECT_TRUE(charInventory.isTwoHandedEquipped());
    
    // Test that off-hand is blocked when two-handed weapon is equipped
    auto shield = std::make_shared<Item>("Shield", ItemType::ARMOR);
    shield->setEquipmentSlot(EquipmentSlot::OFF_HAND);
    shield->setSize(2, 3);
    
    EXPECT_FALSE(charInventory.equipItemWithValidation(shield));
    
    // Test character level requirements
    charInventory.setCharacterLevel(10);
    
    auto highLevelHelm = std::make_shared<Item>("Crown of Ages", ItemType::ARMOR);
    highLevelHelm->setEquipmentSlot(EquipmentSlot::HEAD);
    highLevelHelm->setRequiredLevel(80);
    
    EXPECT_FALSE(charInventory.equipItemWithValidation(highLevelHelm));
    
    // Test character stat requirements
    charInventory.setCharacterStrength(20);
    
    auto strengthBoots = std::make_shared<Item>("Heavy Boots", ItemType::ARMOR);
    strengthBoots->setEquipmentSlot(EquipmentSlot::FEET);
    strengthBoots->setRequiredStrength(60);
    
    EXPECT_FALSE(charInventory.equipItemWithValidation(strengthBoots));
}

// Test for Phase 4 Enhancement: Item stacking
TEST_F(InventoryTest, ItemStacking) {
    Inventory inventory(10, 4);
    
    // Create stackable items (potions, arrows, etc.)
    auto potion1 = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion1->setSize(1, 1);
    potion1->setStackable(true);
    potion1->setMaxStackSize(20);
    potion1->setQuantity(5);
    
    auto potion2 = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion2->setSize(1, 1);
    potion2->setStackable(true);
    potion2->setMaxStackSize(20);
    potion2->setQuantity(10);
    
    // Test adding stackable item
    EXPECT_TRUE(inventory.addItem(potion1, 0, 0));
    EXPECT_EQ(inventory.getItemAt(0, 0)->getQuantity(), 5);
    
    // Test stacking items of same type
    EXPECT_TRUE(inventory.addStackableItem(potion2));
    auto stackedItem = inventory.getItemAt(0, 0);
    EXPECT_EQ(stackedItem->getQuantity(), 15);  // 5 + 10
    EXPECT_EQ(inventory.getUsedSlots(), 1);     // Still only 1 slot used
    
    // Test stack overflow
    auto potion3 = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion3->setSize(1, 1);
    potion3->setStackable(true);
    potion3->setMaxStackSize(20);
    potion3->setQuantity(10);  // Would make 25 total, exceeding max of 20
    
    EXPECT_TRUE(inventory.addStackableItem(potion3));
    EXPECT_EQ(inventory.getItemAt(0, 0)->getQuantity(), 20);  // Maxed at 20
    EXPECT_EQ(inventory.getItemAt(1, 0)->getQuantity(), 5);   // Overflow in new stack
    EXPECT_EQ(inventory.getUsedSlots(), 2);
    
    // Test non-stackable items don't stack
    auto uniquePotion = std::make_shared<Item>("Unique Potion", ItemType::CONSUMABLE);
    uniquePotion->setSize(1, 1);
    uniquePotion->setStackable(false);
    
    EXPECT_TRUE(inventory.addItem(uniquePotion, 2, 0));
    EXPECT_EQ(inventory.getUsedSlots(), 3);
    
    // Test different item types don't stack
    auto manaPotion = std::make_shared<Item>("Mana Potion", ItemType::CONSUMABLE);
    manaPotion->setSize(1, 1);
    manaPotion->setStackable(true);
    manaPotion->setMaxStackSize(20);
    manaPotion->setQuantity(5);
    
    EXPECT_TRUE(inventory.addStackableItem(manaPotion));
    EXPECT_EQ(inventory.getUsedSlots(), 4);  // New stack for different potion
    
    // Test splitting stacks
    auto splitStack = inventory.splitStack(0, 0, 8);  // Split 8 from the 20
    EXPECT_NE(splitStack, nullptr);
    EXPECT_EQ(splitStack->getQuantity(), 8);
    EXPECT_EQ(inventory.getItemAt(0, 0)->getQuantity(), 12);  // 20 - 8
    
    // Test merging stacks
    EXPECT_TRUE(inventory.addItem(splitStack, 0, 1));
    EXPECT_TRUE(inventory.mergeStacks(0, 1, 0, 0));  // Merge back
    EXPECT_EQ(inventory.getItemAt(0, 0)->getQuantity(), 20);  // Back to max
    EXPECT_EQ(inventory.getItemAt(0, 1), nullptr);  // Source slot now empty
}