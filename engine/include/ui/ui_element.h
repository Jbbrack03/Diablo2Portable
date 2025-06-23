#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include "ui/touch_input.h"

namespace d2 {

class UIElement {
public:
    UIElement() = default;
    virtual ~UIElement() = default;
    
    // Position and size
    glm::vec2 getPosition() const { return position_; }
    glm::vec2 getSize() const { return size_; }
    void setPosition(const glm::vec2& pos) { position_ = pos; }
    void setSize(const glm::vec2& size) { size_ = size; }
    
    // State
    bool isVisible() const { return visible_; }
    bool isEnabled() const { return enabled_; }
    bool isFocused() const { return focused_; }
    void setFocused(bool focused) { focused_ = focused; }
    
    // Touch input
    virtual bool handleTouchInput(float x, float y, TouchEventType type);
    
    // Visual styling
    glm::vec4 getBackgroundColor() const { return background_color_; }
    void setBackgroundColor(const glm::vec4& color) { background_color_ = color; }
    
    uint32_t getBackgroundSprite() const { return background_sprite_; }
    void setBackgroundSprite(uint32_t textureId) { background_sprite_ = textureId; }
    
private:
    glm::vec2 position_{0.0f, 0.0f};
    glm::vec2 size_{0.0f, 0.0f};
    bool visible_{true};
    bool enabled_{true};
    bool focused_{false};
    glm::vec4 background_color_{0.0f, 0.0f, 0.0f, 0.0f}; // Default transparent
    uint32_t background_sprite_{0}; // 0 = no sprite
};

} // namespace d2