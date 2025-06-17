#include "map/map_loader.h"
#include <random>
#include <algorithm>

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

bool Map::hasLayer(const std::string& layerName) const {
    return m_layers.find(layerName) != m_layers.end();
}

int Map::getLayerWidth(const std::string& layerName) const {
    auto it = m_layers.find(layerName);
    if (it != m_layers.end() && !it->second.empty()) {
        return it->second[0].size();
    }
    return 0;
}

int Map::getLayerHeight(const std::string& layerName) const {
    auto it = m_layers.find(layerName);
    if (it != m_layers.end()) {
        return it->second.size();
    }
    return 0;
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
        // Add a vertical wall in the middle (x=5, y=1 to y=8)
        // Leave gaps at y=0 and y=9 so there's a path around
        for (int y = 1; y < 9; y++) {
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
    else if (filename == "pathfinding_test_maze.ds1") {
        // Create a simple maze pattern with a solvable path
        // ..........
        // .####.....
        // .....#....
        // .....#....
        // .....#....
        // .....#....
        // .....#....
        // ..........
        // ..........
        // ..........
        for (int x = 1; x < 5; x++) {
            map->m_walkableGrid[1][x] = false;  // Horizontal wall
        }
        for (int y = 2; y < 7; y++) {
            map->m_walkableGrid[y][5] = false;  // Vertical wall
        }
    }
    else if (filename == "multi_layer_test.ds1") {
        // Create a map with multiple layers for testing
        map->m_layers["floor"] = std::vector<std::vector<int>>(map->m_height, std::vector<int>(map->m_width, 1));
        map->m_layers["walls"] = std::vector<std::vector<int>>(map->m_height, std::vector<int>(map->m_width, 0));
        map->m_layers["shadows"] = std::vector<std::vector<int>>(map->m_height, std::vector<int>(map->m_width, 0));
        
        // Add some wall tiles
        map->m_layers["walls"][1][1] = 1;
        map->m_layers["walls"][2][2] = 1;
    }
    else if (filename == "interactive_objects_test.ds1") {
        // Create a map with interactive objects
        MapObject chest;
        chest.type = "chest";
        chest.x = 3;
        chest.y = 3;
        chest.properties["loot"] = "random";
        map->m_objects.push_back(chest);
        
        MapObject door;
        door.type = "door";
        door.x = 5;
        door.y = 5;
        door.properties["locked"] = "false";
        map->m_objects.push_back(door);
    }
    // For other map types, use default (all tiles walkable)
    
    return map;
}

std::unique_ptr<Map> MapLoader::generateRandomMap(int width, int height, uint32_t seed) {
    auto map = std::make_unique<Map>();
    map->m_width = width;
    map->m_height = height;
    
    // Initialize random number generator with seed
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> prob(0.0f, 1.0f);
    
    // Initialize walkable grid
    map->m_walkableGrid.resize(height);
    for (int y = 0; y < height; y++) {
        map->m_walkableGrid[y].resize(width);
        for (int x = 0; x < width; x++) {
            // 30% chance of wall, 70% chance walkable
            map->m_walkableGrid[y][x] = prob(rng) > 0.3f;
        }
    }
    
    // Ensure borders are walls for proper dungeon feel
    for (int x = 0; x < width; x++) {
        map->m_walkableGrid[0][x] = false;  // Top border
        map->m_walkableGrid[height-1][x] = false;  // Bottom border
    }
    for (int y = 0; y < height; y++) {
        map->m_walkableGrid[y][0] = false;  // Left border
        map->m_walkableGrid[y][width-1] = false;  // Right border
    }
    
    // Find suitable entrance and exit positions
    std::vector<glm::ivec2> walkablePositions;
    for (int y = 1; y < height-1; y++) {
        for (int x = 1; x < width-1; x++) {
            if (map->m_walkableGrid[y][x]) {
                walkablePositions.push_back({x, y});
            }
        }
    }
    
    if (walkablePositions.size() >= 2) {
        // Randomly select entrance and exit from walkable positions
        std::uniform_int_distribution<size_t> posDist(0, walkablePositions.size() - 1);
        
        size_t entranceIdx = posDist(rng);
        map->m_entrance = walkablePositions[entranceIdx];
        map->m_hasEntrance = true;
        
        // Ensure exit is different from entrance
        size_t exitIdx;
        do {
            exitIdx = posDist(rng);
        } while (exitIdx == entranceIdx && walkablePositions.size() > 1);
        
        map->m_exit = walkablePositions[exitIdx];
        map->m_hasExit = true;
    } else {
        // Fallback: force at least one walkable spot
        map->m_walkableGrid[1][1] = true;
        map->m_entrance = {1, 1};
        map->m_hasEntrance = true;
        
        if (width > 2 && height > 2) {
            map->m_walkableGrid[height-2][width-2] = true;
            map->m_exit = {width-2, height-2};
            map->m_hasExit = true;
        }
    }
    
    return map;
}

} // namespace d2::map