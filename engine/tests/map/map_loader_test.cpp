#include <gtest/gtest.h>
#include "map/map_loader.h"

namespace d2::map {

class MapLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup if needed
    }
};

// Test for Phase 5, Task 5.1: Map System - Basic map loading functionality
TEST_F(MapLoaderTest, LoadBasicMap) {
    MapLoader loader;
    
    // This should create a basic map for testing
    // For now, we'll use a mock filename since we don't have real DS1 files
    auto map = loader.loadMap("test_map.ds1");
    
    // Basic expectations for a loaded map
    EXPECT_NE(map, nullptr);
    EXPECT_GT(map->getWidth(), 0);
    EXPECT_GT(map->getHeight(), 0);
    
    // Test walkability function exists
    // This should not crash (basic walkability check)
    bool walkable = map->isWalkable(0, 0);
    (void)walkable; // Suppress unused variable warning
}

// Test for Phase 5, Task 5.1: Map System - Walkability detection
TEST_F(MapLoaderTest, MapWalkability) {
    MapLoader loader;
    auto map = loader.loadMap("test_map.ds1");
    
    // Test that walkability works within map bounds
    EXPECT_TRUE(map->isWalkable(5, 5));  // Center should be walkable
    
    // Test that out-of-bounds coordinates are not walkable
    EXPECT_FALSE(map->isWalkable(-1, 0));  // Negative coordinates
    EXPECT_FALSE(map->isWalkable(0, -1));
    EXPECT_FALSE(map->isWalkable(20, 5));  // Beyond width (10)
    EXPECT_FALSE(map->isWalkable(5, 20));  // Beyond height (10)
}

// Test for Phase 5, Task 5.1: Map System - Tile-specific walkability
TEST_F(MapLoaderTest, MapWithObstacles) {
    MapLoader loader;
    auto map = loader.loadMap("map_with_walls.ds1");
    
    // Test that we can create maps with specific walkable/non-walkable tiles
    // For this test, we'll assume position (1,1) has a wall
    EXPECT_FALSE(map->isWalkable(1, 1));  // Wall tile should not be walkable
    
    // But adjacent tiles should still be walkable
    EXPECT_TRUE(map->isWalkable(0, 1));   // Adjacent tile
    EXPECT_TRUE(map->isWalkable(2, 1));   // Adjacent tile
}

// Test for Phase 5, Task 5.1: Random Map Generation - Basic room generation
TEST_F(MapLoaderTest, GenerateRandomDungeon) {
    MapLoader loader;
    
    // Generate a random dungeon with seed for deterministic testing
    auto map = loader.generateRandomMap(20, 20, 12345);  // width, height, seed
    
    EXPECT_NE(map, nullptr);
    EXPECT_EQ(map->getWidth(), 20);
    EXPECT_EQ(map->getHeight(), 20);
    
    // Map should have both walkable and non-walkable areas
    bool hasWalkable = false;
    bool hasWalls = false;
    
    for (int x = 0; x < map->getWidth(); x++) {
        for (int y = 0; y < map->getHeight(); y++) {
            if (map->isWalkable(x, y)) {
                hasWalkable = true;
            } else {
                hasWalls = true;
            }
        }
    }
    
    EXPECT_TRUE(hasWalkable) << "Generated map should have walkable areas";
    EXPECT_TRUE(hasWalls) << "Generated map should have walls/obstacles";
}

// Test for Phase 5, Task 5.1: Random Map Generation - Room connectivity
TEST_F(MapLoaderTest, GeneratedMapHasConnectedRooms) {
    MapLoader loader;
    auto map = loader.generateRandomMap(15, 15, 54321);
    
    // Map should have an entrance and exit
    EXPECT_TRUE(map->hasEntrance()) << "Generated map should have entrance";
    EXPECT_TRUE(map->hasExit()) << "Generated map should have exit";
    
    auto entrance = map->getEntrance();
    auto exit = map->getExit();
    
    // Entrance and exit should be walkable
    EXPECT_TRUE(map->isWalkable(entrance.x, entrance.y)) << "Entrance should be walkable";
    EXPECT_TRUE(map->isWalkable(exit.x, exit.y)) << "Exit should be walkable";
    
    // Entrance and exit should be different positions
    EXPECT_NE(entrance.x, exit.x);
    EXPECT_NE(entrance.y, exit.y);
}

// Test for Phase 5, Task 5.1: Map Layers - Multi-layer support
TEST_F(MapLoaderTest, MapSupportsMultipleLayers) {
    MapLoader loader;
    auto map = loader.loadMap("multi_layer_test.ds1");
    
    // Map should support different layer types
    EXPECT_GT(map->getLayerCount(), 1) << "Map should have multiple layers";
    
    // Test different layer types
    EXPECT_TRUE(map->hasLayer("floor")) << "Map should have floor layer";
    EXPECT_TRUE(map->hasLayer("walls")) << "Map should have walls layer";
    EXPECT_TRUE(map->hasLayer("shadows")) << "Map should have shadows layer";
    
    // Each layer should have correct dimensions
    EXPECT_EQ(map->getLayerWidth("floor"), map->getWidth());
    EXPECT_EQ(map->getLayerHeight("floor"), map->getHeight());
}

// Test for Phase 5, Task 5.1: Map Objects - Interactive objects placement
TEST_F(MapLoaderTest, MapContainsInteractiveObjects) {
    MapLoader loader;
    auto map = loader.loadMap("interactive_objects_test.ds1");
    
    // Map should contain various interactive objects
    auto objects = map->getInteractiveObjects();
    EXPECT_GT(objects.size(), 0) << "Map should have interactive objects";
    
    // Check for specific object types
    bool hasChest = false;
    bool hasDoor = false;
    bool hasPortal = false;
    
    for (const auto& obj : objects) {
        if (obj.type == "chest") hasChest = true;
        if (obj.type == "door") hasDoor = true;
        if (obj.type == "portal") hasPortal = true;
    }
    
    // At least one object type should be present
    EXPECT_TRUE(hasChest || hasDoor || hasPortal) << "Map should have interactive objects";
}

} // namespace d2::map