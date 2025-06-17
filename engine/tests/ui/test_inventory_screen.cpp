#include <gtest/gtest.h>
#include <memory>
#include "ui/inventory_screen.h"
#include "game/item.h"

namespace d2 {

using namespace d2::game;

class InventoryScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        inventory_screen_ = std::make_unique<InventoryScreen>();
    }
    
    std::unique_ptr<InventoryScreen> inventory_screen_;
};

TEST_F(InventoryScreenTest, CreateInventoryScreen) {
    // InventoryScreen should inherit from UIElement
    EXPECT_TRUE(inventory_screen_->isVisible());
    EXPECT_TRUE(inventory_screen_->isEnabled());
    
    // Should have default inventory grid size
    EXPECT_EQ(inventory_screen_->getInventoryWidth(), 10);
    EXPECT_EQ(inventory_screen_->getInventoryHeight(), 4);
}

TEST_F(InventoryScreenTest, AddItemToInventory) {
    // Create a test item
    auto sword = Item::createItem("Test Sword", ItemType::WEAPON, ItemRarity::NORMAL, 1);
    sword->setSize(glm::ivec2(1, 3)); // 1x3 sword
    
    // Add item to inventory at position (0, 0)
    bool added = inventory_screen_->addItem(sword, glm::ivec2(0, 0));
    EXPECT_TRUE(added);
    
    // Check item is at the correct position
    auto retrieved = inventory_screen_->getItemAt(glm::ivec2(0, 0));
    EXPECT_EQ(retrieved, sword);
    
    // Check item occupies multiple slots
    auto slot1 = inventory_screen_->getItemAt(glm::ivec2(0, 1));
    auto slot2 = inventory_screen_->getItemAt(glm::ivec2(0, 2));
    EXPECT_EQ(slot1, sword); // Same item in slot below
    EXPECT_EQ(slot2, sword); // Same item in slot below that
}

TEST_F(InventoryScreenTest, MoveItemInInventory) {
    auto potion = Item::createItem("Health Potion", ItemType::CONSUMABLE, ItemRarity::NORMAL, 1);
    potion->setSize(glm::ivec2(1, 1)); // 1x1 potion
    
    // Add potion at (0, 0)
    inventory_screen_->addItem(potion, glm::ivec2(0, 0));
    
    // Select the potion for movement
    inventory_screen_->selectSlot(0, 0);
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(0, 0));
    
    // Move to position (5, 2)
    inventory_screen_->moveSelection(5, 2);
    inventory_screen_->confirmMove();
    
    // Check item moved to new position
    EXPECT_EQ(inventory_screen_->getItemAt(glm::ivec2(5, 2)), potion);
    EXPECT_EQ(inventory_screen_->getItemAt(glm::ivec2(0, 0)), nullptr);
}

TEST_F(InventoryScreenTest, PreventInvalidItemPlacement) {
    auto sword = Item::createItem("Long Sword", ItemType::WEAPON, ItemRarity::NORMAL, 1);
    sword->setSize(glm::ivec2(1, 4)); // 1x4 sword (too tall)
    
    // Try to place sword at bottom row (won't fit)
    bool added = inventory_screen_->addItem(sword, glm::ivec2(0, 1));
    EXPECT_FALSE(added); // Should fail due to insufficient space
    
    // Try to place sword at position that goes out of bounds
    added = inventory_screen_->addItem(sword, glm::ivec2(0, 2));
    EXPECT_FALSE(added); // Should fail due to out of bounds
}

TEST_F(InventoryScreenTest, HandleControllerNavigation) {
    // Start with no selection
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(-1, -1));
    
    // Navigate to first slot
    inventory_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(0, 0));
    
    // Navigate right
    inventory_screen_->handleControllerInput(ControllerAction::NAVIGATE_RIGHT);
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(1, 0));
    
    // Navigate down
    inventory_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(1, 1));
    
    // Navigate left
    inventory_screen_->handleControllerInput(ControllerAction::NAVIGATE_LEFT);
    EXPECT_EQ(inventory_screen_->getSelectedPosition(), glm::ivec2(0, 1));
}

} // namespace d2