#pragma once

#include <glm/glm.hpp>
#include <cstdint>

namespace d2::game {

using EntityId = uint32_t;

class Entity {
public:
    Entity();
    virtual ~Entity() = default;
    
    EntityId getId() const { return id_; }
    glm::vec2 getPosition() const { return position_; }
    void setPosition(const glm::vec2& pos) { position_ = pos; }
    
protected:
    EntityId id_;
    glm::vec2 position_ = glm::vec2(0.0f, 0.0f);
    
private:
    static EntityId nextId_;
};

} // namespace d2::game