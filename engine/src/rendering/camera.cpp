#include "rendering/camera.h"
#include "game/entity.h"

namespace d2::rendering {

Camera::Camera(int screenWidth, int screenHeight)
    : screenWidth_(screenWidth)
    , screenHeight_(screenHeight)
    , center_(screenWidth / 2.0f, screenHeight / 2.0f) {
}

void Camera::followTarget(const d2::game::Entity* target) {
    target_ = target;
}

void Camera::update() {
    if (target_) {
        // Center the camera on the target
        center_ = target_->getPosition();
    }
}

glm::vec2 Camera::getCenter() const {
    return center_;
}

} // namespace d2::rendering