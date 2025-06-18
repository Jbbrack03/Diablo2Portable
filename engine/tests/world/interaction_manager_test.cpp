#include <gtest/gtest.h>
#include "world/interaction_manager.h"
#include "world/world_object.h"

using namespace d2::world;

class InteractionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<InteractionManager>();
    }
    
    std::unique_ptr<InteractionManager> manager;
};

// Test adding objects to the manager
TEST_F(InteractionManagerTest, AddObject) {
    auto door = std::make_unique<Door>(100, 200);
    
    int id = manager->addObject(std::move(door));
    
    EXPECT_GT(id, 0);  // Should return positive ID
    
    WorldObject* retrievedObject = manager->getObject(id);
    EXPECT_NE(retrievedObject, nullptr);
    EXPECT_EQ(retrievedObject->getType(), ObjectType::DOOR);
    EXPECT_EQ(retrievedObject->getPositionX(), 100);
    EXPECT_EQ(retrievedObject->getPositionY(), 200);
}

// Test adding multiple objects gets different IDs
TEST_F(InteractionManagerTest, AddMultipleObjectsGetDifferentIds) {
    auto door = std::make_unique<Door>(100, 200);
    auto chest = std::make_unique<Chest>(300, 400);
    
    int doorId = manager->addObject(std::move(door));
    int chestId = manager->addObject(std::move(chest));
    
    EXPECT_NE(doorId, chestId);
    EXPECT_GT(doorId, 0);
    EXPECT_GT(chestId, 0);
    
    // Both objects should be retrievable
    EXPECT_NE(manager->getObject(doorId), nullptr);
    EXPECT_NE(manager->getObject(chestId), nullptr);
}

// Test getting non-existent object returns nullptr
TEST_F(InteractionManagerTest, GetNonExistentObject) {
    EXPECT_EQ(manager->getObject(999), nullptr);
}

// Test removing object
TEST_F(InteractionManagerTest, RemoveObject) {
    auto door = std::make_unique<Door>(100, 200);
    int id = manager->addObject(std::move(door));
    
    // Object should exist
    EXPECT_NE(manager->getObject(id), nullptr);
    
    // Remove the object
    manager->removeObject(id);
    
    // Object should no longer exist
    EXPECT_EQ(manager->getObject(id), nullptr);
}

// Test removing non-existent object doesn't crash
TEST_F(InteractionManagerTest, RemoveNonExistentObject) {
    // Should not crash
    manager->removeObject(999);
}

// Test getObjectsInRange functionality
TEST_F(InteractionManagerTest, GetObjectsInRange) {
    // Add objects at different distances from player at (100, 100)
    auto nearDoor = std::make_unique<Door>(105, 105);  // Distance ~7
    auto farDoor = std::make_unique<Door>(150, 150);   // Distance ~70
    auto mediumChest = std::make_unique<Chest>(110, 110);  // Distance ~14
    
    int nearId = manager->addObject(std::move(nearDoor));
    int farId = manager->addObject(std::move(farDoor));
    int mediumId = manager->addObject(std::move(mediumChest));
    
    // Search with range 20 from player at (100, 100)
    auto nearbyObjects = manager->getObjectsInRange(100, 100, 20);
    
    // Should find near door and medium chest, but not far door
    EXPECT_EQ(nearbyObjects.size(), 2);
    EXPECT_TRUE(std::find(nearbyObjects.begin(), nearbyObjects.end(), nearId) != nearbyObjects.end());
    EXPECT_TRUE(std::find(nearbyObjects.begin(), nearbyObjects.end(), mediumId) != nearbyObjects.end());
    EXPECT_FALSE(std::find(nearbyObjects.begin(), nearbyObjects.end(), farId) != nearbyObjects.end());
}

// Test getObjectsInRange with no objects in range
TEST_F(InteractionManagerTest, GetObjectsInRangeEmpty) {
    auto farDoor = std::make_unique<Door>(1000, 1000);
    manager->addObject(std::move(farDoor));
    
    auto nearbyObjects = manager->getObjectsInRange(0, 0, 10);
    EXPECT_TRUE(nearbyObjects.empty());
}

// Test canInteractWith with different object types
TEST_F(InteractionManagerTest, CanInteractWithDoor) {
    auto door = std::make_unique<Door>(100, 200);
    int id = manager->addObject(std::move(door));
    
    // Door should be interactable when unlocked and closed
    EXPECT_TRUE(manager->canInteractWith(id));
}

// Test canInteractWith non-existent object
TEST_F(InteractionManagerTest, CanInteractWithNonExistentObject) {
    EXPECT_FALSE(manager->canInteractWith(999));
}

// Test interaction with door
TEST_F(InteractionManagerTest, InteractWithDoor) {
    auto door = std::make_unique<Door>(100, 200);
    Door* doorPtr = door.get();  // Keep reference before moving
    int id = manager->addObject(std::move(door));
    
    // Door should start closed
    EXPECT_FALSE(doorPtr->isOpen());
    
    // Interact with door
    manager->interactWith(id);
    
    // Door should now be open
    EXPECT_TRUE(doorPtr->isOpen());
}

// Test interaction with chest
TEST_F(InteractionManagerTest, InteractWithChest) {
    auto chest = std::make_unique<Chest>(300, 400);
    chest->addItem("Gold", 100);
    chest->addItem("Potion", 5);
    Chest* chestPtr = chest.get();
    int id = manager->addObject(std::move(chest));
    
    // Chest should have items and be interactable initially
    EXPECT_TRUE(chestPtr->hasItems());
    EXPECT_EQ(chestPtr->getItemCount(), 2);
    EXPECT_TRUE(manager->canInteractWith(id));
    
    // Interact with chest (opens it)
    manager->interactWith(id);
    
    // Chest should no longer be interactable after opening
    EXPECT_FALSE(manager->canInteractWith(id));
    EXPECT_FALSE(chestPtr->hasItems());  // Items should be gone
}

// Test interaction with non-existent object
TEST_F(InteractionManagerTest, InteractWithNonExistentObject) {
    // Should not crash
    manager->interactWith(999);
}

// Test shrine proximity effects
TEST_F(InteractionManagerTest, ShrineProximityEffect) {
    auto shrine = std::make_unique<Shrine>(100, 100);
    shrine->setEffectRadius(15);
    Shrine* shrinePtr = shrine.get();
    int id = manager->addObject(std::move(shrine));
    
    // Player within range
    EXPECT_TRUE(manager->isPlayerInRange(id, 105, 105));  // Distance ~7
    EXPECT_FALSE(manager->isPlayerInRange(id, 130, 130)); // Distance ~42
    
    // Shrine should not be activated initially
    EXPECT_FALSE(shrinePtr->hasBeenActivated());
    
    // Activate proximity effect
    manager->activateProximityEffect(id, 105, 105);
    
    // Shrine should now be activated
    EXPECT_TRUE(shrinePtr->hasBeenActivated());
}

// Test shrine proximity effect outside range
TEST_F(InteractionManagerTest, ShrineProximityEffectOutsideRange) {
    auto shrine = std::make_unique<Shrine>(100, 100);
    shrine->setEffectRadius(10);
    Shrine* shrinePtr = shrine.get();
    int id = manager->addObject(std::move(shrine));
    
    // Try to activate from outside range
    manager->activateProximityEffect(id, 150, 150);
    
    // Shrine should not be activated
    EXPECT_FALSE(shrinePtr->hasBeenActivated());
}

// Test shrine doesn't activate twice
TEST_F(InteractionManagerTest, ShrineActivateOnlyOnce) {
    auto shrine = std::make_unique<Shrine>(100, 100);
    shrine->setEffectRadius(15);
    Shrine* shrinePtr = shrine.get();
    int id = manager->addObject(std::move(shrine));
    
    // First activation
    manager->activateProximityEffect(id, 105, 105);
    EXPECT_TRUE(shrinePtr->hasBeenActivated());
    
    // Try to activate again - should not change anything
    manager->activateProximityEffect(id, 105, 105);
    EXPECT_TRUE(shrinePtr->hasBeenActivated());  // Still activated
}

// Test isPlayerInRange with non-shrine objects
TEST_F(InteractionManagerTest, IsPlayerInRangeNonShrine) {
    auto door = std::make_unique<Door>(100, 100);
    int id = manager->addObject(std::move(door));
    
    // Non-shrine objects should return false for isPlayerInRange
    EXPECT_FALSE(manager->isPlayerInRange(id, 105, 105));
}

// Test isPlayerInRange with non-existent object
TEST_F(InteractionManagerTest, IsPlayerInRangeNonExistentObject) {
    EXPECT_FALSE(manager->isPlayerInRange(999, 100, 100));
}

// Test portal functionality
TEST_F(InteractionManagerTest, PortalInteraction) {
    auto portal = std::make_unique<Portal>(200, 300);
    portal->setDestination("Act2", 500, 600);
    portal->setActive(true);
    Portal* portalPtr = portal.get();
    int id = manager->addObject(std::move(portal));
    
    // Portal should be interactable when active
    EXPECT_TRUE(manager->canInteractWith(id));
    
    // Check destination
    EXPECT_EQ(portalPtr->getDestinationMap(), "Act2");
    EXPECT_EQ(portalPtr->getDestination().x, 500);
    EXPECT_EQ(portalPtr->getDestination().y, 600);
}

// Test lever functionality
TEST_F(InteractionManagerTest, LeverInteraction) {
    auto lever = std::make_unique<Lever>(250, 350);
    lever->setTargetObject(42);
    Lever* leverPtr = lever.get();
    int id = manager->addObject(std::move(lever));
    
    // Lever should be interactable
    EXPECT_TRUE(manager->canInteractWith(id));
    
    // Lever should not be activated initially
    EXPECT_FALSE(leverPtr->isActivated());
    
    // Interact with lever
    manager->interactWith(id);
    
    // Lever should now be activated
    EXPECT_TRUE(leverPtr->isActivated());
    EXPECT_EQ(leverPtr->getTargetObjectId(), 42);
}