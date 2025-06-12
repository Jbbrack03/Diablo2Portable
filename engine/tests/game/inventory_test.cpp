#include <gtest/gtest.h>
#include <memory>
#include "game/inventory.h"
#include "game/item.h"

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