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

} // namespace d2::map