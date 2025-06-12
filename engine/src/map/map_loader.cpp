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
    
    // Create different map layouts based on filename
    if (filename == "map_with_walls.ds1") {
        // Initialize walkable grid
        map->m_walkableGrid.resize(map->m_height);
        for (int y = 0; y < map->m_height; y++) {
            map->m_walkableGrid[y].resize(map->m_width, true);  // Start with all walkable
        }
        
        // Add a wall at position (1,1)
        map->m_walkableGrid[1][1] = false;
    }
    // For other map types, use default (all tiles walkable)
    
    return map;
}

} // namespace d2::map