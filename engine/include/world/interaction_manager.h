#pragma once

#include "world/world_object.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace d2::world {

class InteractionManager {
public:
    InteractionManager() = default;
    ~InteractionManager() = default;
    
    // Object management
    int addObject(std::unique_ptr<WorldObject> object);
    WorldObject* getObject(int objectId);
    void removeObject(int objectId);
    
    // Proximity and interaction
    std::vector<int> getObjectsInRange(int playerX, int playerY, int range);
    bool canInteractWith(int objectId);
    void interactWith(int objectId);
    
    // Proximity effects
    bool isPlayerInRange(int objectId, int playerX, int playerY);
    void activateProximityEffect(int objectId, int playerX, int playerY);
    
private:
    std::unordered_map<int, std::unique_ptr<WorldObject>> m_objects;
    int m_nextObjectId = 1;
    
    int calculateDistanceSquared(int x1, int y1, int x2, int y2);
};

} // namespace d2::world