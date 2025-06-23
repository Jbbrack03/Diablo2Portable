#include "performance/optimized_update_system.h"
#include "game/game_state.h"
#include "game/player.h"
#include "game/monster.h"
#include <glm/geometric.hpp>
#include <algorithm>

namespace d2::performance {

OptimizedUpdateSystem::OptimizedUpdateSystem() {
}

void OptimizedUpdateSystem::updateEntities(d2::game::GameState& gameState, float deltaTime) {
    if (!optimizationsEnabled_) {
        // Fallback to simple update
        const auto& monsters = gameState.getAllMonsters();
        for (const auto& [id, monster] : monsters) {
            if (monster) {
                monster->updateAI();
            }
        }
        return;
    }
    
    // Get player position for LOD calculations
    glm::vec2 playerPos(0, 0);
    if (gameState.hasPlayer()) {
        auto player = gameState.getPlayer();
        if (player) {
            playerPos = player->getPosition();
        }
    }
    
    // Collect monsters into a vector for batch processing
    std::vector<std::shared_ptr<d2::game::Monster>> monsterList;
    const auto& monsters = gameState.getAllMonsters();
    
    for (const auto& [id, monster] : monsters) {
        if (monster) {
            monsterList.push_back(monster);
        }
    }
    
    // Sort monsters by distance to player for LOD processing
    if (lodEnabled_) {
        std::sort(monsterList.begin(), monsterList.end(),
            [&playerPos](const auto& a, const auto& b) {
                float distA = glm::length(a->getPosition() - playerPos);
                float distB = glm::length(b->getPosition() - playerPos);
                return distA < distB;
            });
    }
    
    // Process monsters in batches
    size_t totalMonsters = monsterList.size();
    for (size_t i = 0; i < totalMonsters; i += batchSize_) {
        size_t endIdx = std::min(i + batchSize_, totalMonsters);
        updateMonsterBatch(monsterList, i, endIdx, deltaTime);
    }
}

void OptimizedUpdateSystem::updateMonsterBatch(
    const std::vector<std::shared_ptr<d2::game::Monster>>& monsters,
    size_t startIdx, size_t endIdx, float deltaTime) {
    
    // Get player position once for the batch
    glm::vec2 playerPos(0, 0);
    bool hasPlayer = false;
    
    // In a real implementation, we'd get this from gameState
    // For now, assume player is at origin
    
    for (size_t i = startIdx; i < endIdx; i++) {
        auto& monster = monsters[i];
        if (!monster) continue;
        
        // Calculate distance for LOD
        float distance = glm::length(monster->getPosition() - playerPos);
        int lodLevel = getLODLevel(distance);
        
        // Update based on LOD level
        switch (lodLevel) {
            case 0: // Near - full update
                monster->updateAI();
                // In a real game, we'd update animations, effects, etc.
                break;
                
            case 1: // Medium - reduced update
                // Update AI every other frame
                if (static_cast<int>(i) % 2 == 0) {
                    monster->updateAI();
                }
                break;
                
            case 2: // Far - minimal update
                // Update AI every 4th frame
                if (static_cast<int>(i) % 4 == 0) {
                    monster->updateAI();
                }
                break;
                
            case 3: // Very far - skip most updates
                // Update AI every 8th frame
                if (static_cast<int>(i) % 8 == 0) {
                    monster->updateAI();
                }
                break;
        }
    }
}

int OptimizedUpdateSystem::getLODLevel(float distance) const {
    if (!lodEnabled_) {
        return 0; // Always full update if LOD disabled
    }
    
    if (distance < lodNearDistance_) {
        return 0; // Near - full update
    } else if (distance < lodMediumDistance_) {
        return 1; // Medium - reduced update
    } else if (distance < lodFarDistance_) {
        return 2; // Far - minimal update
    } else {
        return 3; // Very far - skip most updates
    }
}

} // namespace d2::performance