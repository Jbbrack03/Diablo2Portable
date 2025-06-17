#include "world/interaction_manager.h"

namespace d2::world {

int InteractionManager::addObject(std::unique_ptr<WorldObject> object) {
    int id = m_nextObjectId++;
    m_objects[id] = std::move(object);
    return id;
}

WorldObject* InteractionManager::getObject(int objectId) {
    auto it = m_objects.find(objectId);
    if (it != m_objects.end()) {
        return it->second.get();
    }
    return nullptr;
}

void InteractionManager::removeObject(int objectId) {
    m_objects.erase(objectId);
}

std::vector<int> InteractionManager::getObjectsInRange(int playerX, int playerY, int range) {
    std::vector<int> nearbyObjects;
    int rangeSquared = range * range;
    
    for (const auto& [id, object] : m_objects) {
        int distSquared = calculateDistanceSquared(playerX, playerY, 
                                                   object->getPositionX(), 
                                                   object->getPositionY());
        if (distSquared <= rangeSquared) {
            nearbyObjects.push_back(id);
        }
    }
    
    return nearbyObjects;
}

bool InteractionManager::canInteractWith(int objectId) {
    auto* object = getObject(objectId);
    return object && object->canInteract();
}

void InteractionManager::interactWith(int objectId) {
    auto* object = getObject(objectId);
    if (object && object->canInteract()) {
        object->interact();
    }
}

bool InteractionManager::isPlayerInRange(int objectId, int playerX, int playerY) {
    auto* object = getObject(objectId);
    if (!object) return false;
    
    // For shrines, use their effect radius
    if (object->getType() == ObjectType::SHRINE) {
        auto* shrine = dynamic_cast<Shrine*>(object);
        if (shrine) {
            int distSquared = calculateDistanceSquared(playerX, playerY,
                                                       object->getPositionX(),
                                                       object->getPositionY());
            int radiusSquared = shrine->getEffectRadius() * shrine->getEffectRadius();
            return distSquared <= radiusSquared;
        }
    }
    
    return false;
}

void InteractionManager::activateProximityEffect(int objectId, int playerX, int playerY) {
    if (isPlayerInRange(objectId, playerX, playerY)) {
        auto* object = getObject(objectId);
        if (object && object->getType() == ObjectType::SHRINE) {
            auto* shrine = dynamic_cast<Shrine*>(object);
            if (shrine && !shrine->hasBeenActivated()) {
                shrine->activate();
            }
        }
    }
}

int InteractionManager::calculateDistanceSquared(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return dx * dx + dy * dy;
}

} // namespace d2::world