#pragma once

#include "ui/ui_element.h"
#include <string>

namespace d2 {

class UIButton : public UIElement {
public:
    explicit UIButton(const std::string& text) : text_(text) {}
    virtual ~UIButton() = default;
    
    // Text
    const std::string& getText() const { return text_; }
    
    // Button state
    bool isPressed() const { return pressed_; }
    bool isHovered() const { return hovered_; }
    
private:
    std::string text_;
    bool pressed_{false};
    bool hovered_{false};
};

} // namespace d2