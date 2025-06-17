#pragma once

#include <glm/glm.hpp>

namespace d2 {

class UIElement {
public:
    UIElement() = default;
    virtual ~UIElement() = default;
    
    // Position and size
    glm::vec2 getPosition() const { return position_; }
    glm::vec2 getSize() const { return size_; }
    
    // State
    bool isVisible() const { return visible_; }
    bool isEnabled() const { return enabled_; }
    bool isFocused() const { return focused_; }
    
private:
    glm::vec2 position_{0.0f, 0.0f};
    glm::vec2 size_{0.0f, 0.0f};
    bool visible_{true};
    bool enabled_{true};
    bool focused_{false};
};

} // namespace d2