#include <gtest/gtest.h>
#include <memory>
#include "game/game_engine.h"
#include "game/game_state.h"
#include "game/monster.h"
#include "game/item.h"
#include "game/entity.h"
#include "game/dropped_item.h"
#include "game/loot_system.h"
#include "game/player.h"
#include "game/character.h"
#include "game/inventory.h"

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

// Test 4: When monster dies, loot should be generated and added to world
TEST_F(ItemDropIntegrationTest, MonsterDeathGeneratesLoot) {
    // Configure loot system for basic drops
    auto* lootSystem = engine->getLootSystem();
    std::vector<LootTableEntry> lootTable = {
        {ItemType::WEAPON, 0.5f},
        {ItemType::GOLD, 1.0f}  // Always drop gold
    };
    lootSystem->setMonsterLootTable(MonsterType::SKELETON, lootTable);
    
    // Add a monster to the game
    auto skeleton = std::make_shared<Monster>(MonsterType::SKELETON, 5);
    skeleton->setPosition(100, 100);
    EntityId monsterId = gameState->addMonster(skeleton);
    
    // Simulate monster death
    skeleton->takeDamage(skeleton->getLife() + 1);  // Deal enough damage to kill
    EXPECT_LE(skeleton->getCurrentLife(), 0);
    
    // Process monster death in game engine
    engine->processMonsterDeath(monsterId);
    
    // Check that loot was generated and added to the world
    const auto& droppedItems = gameState->getAllDroppedItems();
    EXPECT_GT(droppedItems.size(), 0);
    
    // Verify at least one gold drop
    bool foundGold = false;
    for (const auto& [id, droppedItem] : droppedItems) {
        if (droppedItem->getItem()->getType() == ItemType::GOLD) {
            foundGold = true;
            // Items should be dropped near the monster's position
            auto pos = droppedItem->getPosition();
            EXPECT_NEAR(pos.x, 100.0f, 50.0f);  // Within 50 units
            EXPECT_NEAR(pos.y, 100.0f, 50.0f);
        }
    }
    EXPECT_TRUE(foundGold);
}

// Test 5: Player should pick up items when walking over them
TEST_F(ItemDropIntegrationTest, PlayerPicksUpDroppedItems) {
    // Add a player to the game
    Character character(CharacterClass::BARBARIAN);
    auto player = std::make_shared<Player>(character);
    player->setPosition(glm::vec2(50.0f, 50.0f));
    gameState->setPlayer(player);
    
    // Create and drop an item at a specific location
    auto potion = std::make_shared<Item>("Health Potion", ItemType::CONSUMABLE);
    potion->setSize(1, 1);  // Small item
    auto droppedItem = std::make_shared<DroppedItem>(potion, glm::vec2(60.0f, 60.0f));
    EntityId droppedId = gameState->addDroppedItem(droppedItem);
    
    // Verify item is in the world
    EXPECT_EQ(gameState->getAllDroppedItems().size(), 1);
    
    // Move player to the item location
    player->setPosition(glm::vec2(60.0f, 60.0f));
    
    // Process item pickup
    engine->processItemPickup();
    
    // Item should be removed from world
    EXPECT_EQ(gameState->getAllDroppedItems().size(), 0);
    EXPECT_EQ(gameState->getDroppedItem(droppedId), nullptr);
    
    // For now, we'll just verify the item was removed from the world
    // In a full implementation, we'd add a method to Player to check inventory
    // or expose the character's inventory
    // TODO: Add inventory checking once Player interface is extended
}