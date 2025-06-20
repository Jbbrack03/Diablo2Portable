#pragma once

#include <glm/glm.hpp>

namespace d2::game {

class Entity {
public:
    virtual ~Entity() = default;
    
    glm::vec2 getPosition() const { return position_; }
    void setPosition(const glm::vec2& pos) { position_ = pos; }
    
protected:
    glm::vec2 position_ = glm::vec2(0.0f, 0.0f);
};

} // namespace d2::game