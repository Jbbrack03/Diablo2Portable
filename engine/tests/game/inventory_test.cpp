#include <gtest/gtest.h>
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