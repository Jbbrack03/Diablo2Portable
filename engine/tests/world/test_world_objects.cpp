#include <gtest/gtest.h>
#include "world/world_object.h"
#include "world/interaction_manager.h"

using namespace d2::world;

class WorldObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

// Test for Phase 5, Task 5.3: World Interactions - Basic object creation
TEST_F(WorldObjectTest, CreateBasicWorldObjects) {
    Door door(100, 100);
    door.setLocked(false);
    
    EXPECT_EQ(door.getType(), ObjectType::DOOR);
    EXPECT_EQ(door.getPositionX(), 100);
    EXPECT_EQ(door.getPositionY(), 100);
    EXPECT_FALSE(door.isLocked());
    EXPECT_TRUE(door.canInteract());
}

// Test for Phase 5, Task 5.3: World Interactions - Chest functionality
TEST_F(WorldObjectTest, ChestInteraction) {
    Chest treasure(200, 200);
    treasure.addItem("Gold", 500);
    treasure.addItem("Potion", 3);
    
    EXPECT_EQ(treasure.getType(), ObjectType::CHEST);
    EXPECT_TRUE(treasure.hasItems());
    EXPECT_EQ(treasure.getItemCount(), 2);
    
    // Open chest and retrieve items
    auto items = treasure.open();
    EXPECT_EQ(items.size(), 2);
    EXPECT_EQ(items[0].name, "Gold");
    EXPECT_EQ(items[0].quantity, 500);
    
    // Chest should be empty after opening
    EXPECT_FALSE(treasure.hasItems());
    EXPECT_FALSE(treasure.canInteract());
}

// Test for Phase 5, Task 5.3: World Interactions - Portal system
TEST_F(WorldObjectTest, PortalTravel) {
    Portal townPortal(50, 50);
    townPortal.setDestination("Town", 300, 300);
    townPortal.setActive(true);
    
    EXPECT_EQ(townPortal.getType(), ObjectType::PORTAL);
    EXPECT_TRUE(townPortal.isActive());
    EXPECT_EQ(townPortal.getDestinationMap(), "Town");
    
    auto destination = townPortal.getDestination();
    EXPECT_EQ(destination.x, 300);
    EXPECT_EQ(destination.y, 300);
}

// Test for Phase 5, Task 5.3: World Interactions - Lever/switch mechanics
TEST_F(WorldObjectTest, LeverSwitchMechanics) {
    Lever secretLever(150, 150);
    secretLever.setTargetObject(999);  // ID of object to activate
    
    EXPECT_EQ(secretLever.getType(), ObjectType::LEVER);
    EXPECT_FALSE(secretLever.isActivated());
    
    // Activate lever
    secretLever.activate();
    EXPECT_TRUE(secretLever.isActivated());
    EXPECT_EQ(secretLever.getTargetObjectId(), 999);
}

// Test for Phase 5, Task 5.3: World Interactions - Interaction management
TEST_F(WorldObjectTest, InteractionManager) {
    InteractionManager manager;
    
    // Add objects to the world
    auto door = std::make_unique<Door>(100, 100);
    auto chest = std::make_unique<Chest>(200, 200);
    
    int doorId = manager.addObject(std::move(door));
    int chestId = manager.addObject(std::move(chest));
    
    // Find objects near player position
    auto nearbyObjects = manager.getObjectsInRange(95, 95, 10);  // 10 unit range
    EXPECT_EQ(nearbyObjects.size(), 1);  // Should find the door
    EXPECT_EQ(nearbyObjects[0], doorId);
    
    // Interact with object
    EXPECT_TRUE(manager.canInteractWith(doorId));
    manager.interactWith(doorId);
    
    // Check object state changed
    auto* doorObj = manager.getObject(doorId);
    EXPECT_NE(doorObj, nullptr);
}

// Test for Phase 5, Task 5.3: World Interactions - Proximity detection
TEST_F(WorldObjectTest, ProximityDetection) {
    InteractionManager manager;
    
    auto shrine = std::make_unique<Shrine>(250, 250);
    shrine->setEffectRadius(15);
    int shrineId = manager.addObject(std::move(shrine));
    
    // Player too far away
    EXPECT_FALSE(manager.isPlayerInRange(shrineId, 200, 200));
    
    // Player close enough to activate
    EXPECT_TRUE(manager.isPlayerInRange(shrineId, 260, 260));
    
    // Activate shrine effect
    manager.activateProximityEffect(shrineId, 260, 260);
    auto* shrineObj = dynamic_cast<Shrine*>(manager.getObject(shrineId));
    EXPECT_TRUE(shrineObj->hasBeenActivated());
}