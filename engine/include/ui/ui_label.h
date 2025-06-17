#pragma once

#include "ui/ui_element.h"
#include <string>

namespace d2 {

class UILabel : public UIElement {
public:
    enum class Alignment {
        LEFT,
        CENTER,
        RIGHT
    };
    
    explicit UILabel(const std::string& text) : text_(text) {}
    virtual ~UILabel() = default;
    
    const std::string& getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }
    
    Alignment getAlignment() const { return alignment_; }
    void setAlignment(Alignment alignment) { alignment_ = alignment; }
    
private:
    std::string text_;
    Alignment alignment_{Alignment::LEFT};
};

} // namespace d2