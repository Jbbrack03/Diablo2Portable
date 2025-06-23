#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "game/entity_manager.h"

namespace d2::performance {

// Spatial grid for optimizing entity queries and updates
template<typename T>
class SpatialGrid {
public:
    SpatialGrid(float cellSize = 128.0f) : cellSize_(cellSize) {}
    
    // Clear the grid
    void clear() {
        grid_.clear();
    }
    
    // Add an entity to the grid
    void add(game::EntityId id, const glm::vec2& position, T* entity) {
        int gridX = static_cast<int>(position.x / cellSize_);
        int gridY = static_cast<int>(position.y / cellSize_);
        
        uint64_t cellKey = getCellKey(gridX, gridY);
        grid_[cellKey].emplace_back(id, entity);
    }
    
    // Get entities in a specific cell
    std::vector<std::pair<game::EntityId, T*>> getEntitiesInCell(int gridX, int gridY) {
        uint64_t cellKey = getCellKey(gridX, gridY);
        auto it = grid_.find(cellKey);
        if (it != grid_.end()) {
            return it->second;
        }
        return {};
    }
    
    // Get entities within a radius of a position
    std::vector<std::pair<game::EntityId, T*>> getEntitiesInRadius(const glm::vec2& center, float radius) {
        std::vector<std::pair<game::EntityId, T*>> result;
        
        // Calculate grid bounds
        int minX = static_cast<int>((center.x - radius) / cellSize_);
        int maxX = static_cast<int>((center.x + radius) / cellSize_);
        int minY = static_cast<int>((center.y - radius) / cellSize_);
        int maxY = static_cast<int>((center.y + radius) / cellSize_);
        
        // Check all cells in range
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                auto cellEntities = getEntitiesInCell(x, y);
                result.insert(result.end(), cellEntities.begin(), cellEntities.end());
            }
        }
        
        return result;
    }
    
    // Get entities within view bounds (for frustum culling)
    std::vector<std::pair<game::EntityId, T*>> getEntitiesInBounds(
        const glm::vec2& min, const glm::vec2& max) {
        std::vector<std::pair<game::EntityId, T*>> result;
        
        int minX = static_cast<int>(min.x / cellSize_);
        int maxX = static_cast<int>(max.x / cellSize_);
        int minY = static_cast<int>(min.y / cellSize_);
        int maxY = static_cast<int>(max.y / cellSize_);
        
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                auto cellEntities = getEntitiesInCell(x, y);
                result.insert(result.end(), cellEntities.begin(), cellEntities.end());
            }
        }
        
        return result;
    }
    
private:
    float cellSize_;
    std::unordered_map<uint64_t, std::vector<std::pair<game::EntityId, T*>>> grid_;
    
    uint64_t getCellKey(int x, int y) const {
        // Combine x and y into a single 64-bit key
        uint32_t ux = static_cast<uint32_t>(x + 0x7FFFFFFF);
        uint32_t uy = static_cast<uint32_t>(y + 0x7FFFFFFF);
        return (static_cast<uint64_t>(ux) << 32) | uy;
    }
};

} // namespace d2::performance