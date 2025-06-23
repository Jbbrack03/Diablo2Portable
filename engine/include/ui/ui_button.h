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
    
    // Touch input override
    bool handleTouchInput(float x, float y, TouchEventType type) override;
    
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
    
    // Visual state sprites
    uint32_t getNormalSprite() const { return normal_sprite_; }
    void setNormalSprite(uint32_t sprite) { normal_sprite_ = sprite; }
    
    uint32_t getHoverSprite() const { return hover_sprite_; }
    void setHoverSprite(uint32_t sprite) { hover_sprite_ = sprite; }
    
    uint32_t getPressedSprite() const { return pressed_sprite_; }
    void setPressedSprite(uint32_t sprite) { pressed_sprite_ = sprite; }
    
    uint32_t getDisabledSprite() const { return disabled_sprite_; }
    void setDisabledSprite(uint32_t sprite) { disabled_sprite_ = sprite; }
    
    // Get current sprite based on button state
    uint32_t getCurrentSprite() const {
        if (!isEnabled() && disabled_sprite_ != 0) {
            return disabled_sprite_;
        }
        if (pressed_ && pressed_sprite_ != 0) {
            return pressed_sprite_;
        }
        if (hovered_ && hover_sprite_ != 0) {
            return hover_sprite_;
        }
        return normal_sprite_;
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
    
    // Visual state sprites
    uint32_t normal_sprite_{0};
    uint32_t hover_sprite_{0};
    uint32_t pressed_sprite_{0};
    uint32_t disabled_sprite_{0};
};

} // namespace d2