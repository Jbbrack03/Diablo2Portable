#include <gtest/gtest.h>
#include "game/item.h"

using namespace d2::game;

class ItemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup will go here
    }
};

// Test for Phase 4, Task 4.3: Item System - Basic item creation
TEST_F(ItemTest, BasicItemCreation) {
    // Create a basic weapon item
    Item sword("Short Sword", ItemType::WEAPON);
    
    EXPECT_EQ(sword.getName(), "Short Sword");
    EXPECT_EQ(sword.getType(), ItemType::WEAPON);
    EXPECT_EQ(sword.getRequiredLevel(), 1);  // Default level requirement
    
    // Create armor item
    Item armor("Leather Armor", ItemType::ARMOR);
    EXPECT_EQ(armor.getName(), "Leather Armor");
    EXPECT_EQ(armor.getType(), ItemType::ARMOR);
}