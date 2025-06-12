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

} // namespace d2::map