#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include "game/entity_manager.h"

namespace d2::game {
class GameState;
class Monster;
}

namespace d2::performance {

class OptimizedUpdateSystem {
public:
    OptimizedUpdateSystem();
    ~OptimizedUpdateSystem() = default;
    
    // Update all entities with optimizations
    void updateEntities(d2::game::GameState& gameState, float deltaTime);
    
    // Enable/disable optimizations
    void setOptimizationsEnabled(bool enabled) { optimizationsEnabled_ = enabled; }
    bool areOptimizationsEnabled() const { return optimizationsEnabled_; }
    
    // Performance tuning
    void setBatchSize(int size) { batchSize_ = size; }
    int getBatchSize() const { return batchSize_; }
    
    // LOD (Level of Detail) settings
    void setLODEnabled(bool enabled) { lodEnabled_ = enabled; }
    void setLODDistances(float near, float medium, float far) {
        lodNearDistance_ = near;
        lodMediumDistance_ = medium;
        lodFarDistance_ = far;
    }
    
private:
    bool optimizationsEnabled_ = true;
    bool lodEnabled_ = true;
    int batchSize_ = 10; // Process entities in batches
    
    // LOD distances
    float lodNearDistance_ = 200.0f;   // Full update
    float lodMediumDistance_ = 500.0f; // Reduced update
    float lodFarDistance_ = 1000.0f;   // Minimal update
    
    // Update a batch of monsters
    void updateMonsterBatch(const std::vector<std::shared_ptr<d2::game::Monster>>& monsters,
                           size_t startIdx, size_t endIdx, float deltaTime);
    
    // Get LOD level based on distance to player
    int getLODLevel(float distance) const;
};

} // namespace d2::performance