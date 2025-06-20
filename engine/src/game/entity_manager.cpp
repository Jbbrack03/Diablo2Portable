#include "game/entity_manager.h"
#include "game/entity.h"

namespace d2::game {

EntityManager::EntityManager() = default;

EntityManager::~EntityManager() = default;

EntityId EntityManager::addEntity(std::shared_ptr<Entity> entity) {
    if (!entity) {
        return INVALID_ENTITY_ID;
    }
    
    EntityId id = nextId_++;
    entities_[id] = entity;
    return id;
}

} // namespace d2::game