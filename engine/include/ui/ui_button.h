#pragma once

#include "ui/ui_element.h"
#include <string>
#include <functional>

namespace d2 {

class UIButton : public UIElement {
public:
    using ClickCallback = std::function<void()>;
    
    explicit UIButton(const std::string& text) : text_(text) {}
    virtual ~UIButton() = default;
    
    // Text
    const std::string& getText() const { return text_; }
    
    // Button state
    bool isPressed() const { return pressed_; }
    bool isHovered() const { return hovered_; }
    
    // Event handling
    void setOnClick(ClickCallback callback) { onClick_ = callback; }
    
    void handleMouseDown(const glm::vec2& mousePos) {
        if (containsPoint(mousePos)) {
            pressed_ = true;
        }
    }
    
    void handleMouseUp(const glm::vec2& mousePos) {
        if (pressed_ && containsPoint(mousePos)) {
            if (onClick_) {
                onClick_();
            }
        }
        pressed_ = false;
    }
    
private:
    bool containsPoint(const glm::vec2& point) const {
        glm::vec2 pos = getPosition();
        glm::vec2 size = getSize();
        return point.x >= pos.x && point.x <= pos.x + size.x &&
               point.y >= pos.y && point.y <= pos.y + size.y;
    }
    
    std::string text_;
    bool pressed_{false};
    bool hovered_{false};
    ClickCallback onClick_;
};

} // namespace d2