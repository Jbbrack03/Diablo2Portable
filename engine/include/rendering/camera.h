#pragma once

#include <glm/glm.hpp>

namespace d2::game {
class Entity;
}

namespace d2::rendering {

class Camera {
public:
    Camera(int screenWidth, int screenHeight);
    ~Camera() = default;
    
    void followTarget(const d2::game::Entity* target);
    void update();
    
    glm::vec2 getCenter() const;
    
private:
    int screenWidth_;
    int screenHeight_;
    const d2::game::Entity* target_ = nullptr;
    glm::vec2 center_;
};

} // namespace d2::rendering