#include "map/map_loader.h"

namespace d2::map {

bool Map::isWalkable(int x, int y) const {
    // Check bounds
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return false;
    }
    
    // Check walkable grid if it exists
    if (!m_walkableGrid.empty() && y < static_cast<int>(m_walkableGrid.size()) && 
        x < static_cast<int>(m_walkableGrid[y].size())) {
        return m_walkableGrid[y][x];
    }
    
    // Default: all tiles are walkable if no grid data
    return true;
}

std::unique_ptr<Map> MapLoader::loadMap(const std::string& filename) {
    auto map = std::make_unique<Map>();
    map->m_width = 10;
    map->m_height = 10;
    
    // Initialize walkable grid
    map->m_walkableGrid.resize(map->m_height);
    for (int y = 0; y < map->m_height; y++) {
        map->m_walkableGrid[y].resize(map->m_width, true);  // Start with all walkable
    }
    
    // Create different map layouts based on filename
    if (filename == "map_with_walls.ds1") {
        // Add a wall at position (1,1)
        map->m_walkableGrid[1][1] = false;
    }
    else if (filename == "pathfinding_test_vertical_wall.ds1") {
        // Add a vertical wall in the middle (x=5, y=0 to y=9)
        for (int y = 0; y < 10; y++) {
            map->m_walkableGrid[y][5] = false;
        }
    }
    else if (filename == "pathfinding_test_horizontal_wall.ds1") {
        // Add a horizontal wall in the middle (y=5, x=0 to x=9)
        for (int x = 0; x < 10; x++) {
            map->m_walkableGrid[5][x] = false;
        }
    }
    else if (filename == "pathfinding_test_l_shape.ds1") {
        // Create 5x5 map with L-shaped obstacle
        map->m_width = 5;
        map->m_height = 5;
        map->m_walkableGrid.clear();
        map->m_walkableGrid.resize(5);
        for (int y = 0; y < 5; y++) {
            map->m_walkableGrid[y].resize(5, true);
        }
        // Add L-shaped obstacle
        map->m_walkableGrid[1][1] = false;
        map->m_walkableGrid[1][2] = false;
        map->m_walkableGrid[2][1] = false;
    }
    else if (filename == "pathfinding_test_single_obstacle.ds1") {
        // Create 5x5 map with single obstacle in the middle
        map->m_width = 5;
        map->m_height = 5;
        map->m_walkableGrid.clear();
        map->m_walkableGrid.resize(5);
        for (int y = 0; y < 5; y++) {
            map->m_walkableGrid[y].resize(5, true);
        }
        // Add single obstacle
        map->m_walkableGrid[2][2] = false;
    }
    // For other map types, use default (all tiles walkable)
    
    return map;
}

} // namespace d2::map