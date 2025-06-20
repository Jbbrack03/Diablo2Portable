#include <gtest/gtest.h>
#include <memory>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/monster.h"
#include "game/item.h"
#include "game/entity.h"
#include "game/dropped_item.h"
#include "game/loot_system.h"

using namespace d2;
using namespace d2::game;

class ItemDropIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<GameEngine>();
        engine->initialize();
        engine->start();
        gameState = engine->getGameState();
    }
    
    std::unique_ptr<GameEngine> engine;
    GameState* gameState;
};

// Test 1: DroppedItem should be an Entity so it can exist in the world
TEST_F(ItemDropIntegrationTest, DroppedItemIsEntity) {
    // Create a DroppedItem with an item and position
    auto sword = std::make_shared<Item>("Short Sword", ItemType::WEAPON);
    auto droppedItem = std::make_shared<DroppedItem>(sword, glm::vec2(100.0f, 200.0f));
    
    // DroppedItem should be an Entity
    Entity* entity = droppedItem.get();
    EXPECT_NE(entity, nullptr);
    
    // It should have the correct position
    EXPECT_FLOAT_EQ(droppedItem->getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(droppedItem->getPosition().y, 200.0f);
    
    // It should provide access to the contained item
    EXPECT_EQ(droppedItem->getItem(), sword);
}

// Test 2: GameState should manage dropped items
TEST_F(ItemDropIntegrationTest, GameStateManagesDroppedItems) {
    // Create a dropped item
    auto potion = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    auto droppedItem = std::make_shared<DroppedItem>(potion, glm::vec2(50.0f, 75.0f));
    
    // Add it to the game state
    EntityId id = gameState->addDroppedItem(droppedItem);
    EXPECT_NE(id, 0);
    
    // Retrieve it
    auto retrieved = gameState->getDroppedItem(id);
    EXPECT_EQ(retrieved, droppedItem);
    
    // Get all dropped items
    const auto& allDropped = gameState->getAllDroppedItems();
    EXPECT_EQ(allDropped.size(), 1);
    EXPECT_EQ(allDropped.at(id), droppedItem);
}

// Test 3: GameEngine should integrate LootSystem
TEST_F(ItemDropIntegrationTest, GameEngineHasLootSystem) {
    // GameEngine should have a getter for LootSystem
    auto* lootSystem = engine->getLootSystem();
    EXPECT_NE(lootSystem, nullptr);
}