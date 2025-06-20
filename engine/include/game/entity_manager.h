#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>

namespace d2::game {

using EntityId = uint32_t;
constexpr EntityId INVALID_ENTITY_ID = 0;

class Entity;

class EntityManager {
public:
    EntityManager();
    ~EntityManager();
    
    EntityId addEntity(std::shared_ptr<Entity> entity);
    
    size_t getEntityCount() const { return entities_.size(); }
    
private:
    EntityId nextId_ = 1; // Start at 1, 0 is invalid
    std::unordered_map<EntityId, std::shared_ptr<Entity>> entities_;
};

} // namespace d2::game