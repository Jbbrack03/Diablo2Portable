#pragma once

#include "ui/ui_element.h"
#include <string>

namespace d2 {

class UILabel : public UIElement {
public:
    explicit UILabel(const std::string& text) : text_(text) {}
    virtual ~UILabel() = default;
    
    const std::string& getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }
    
private:
    std::string text_;
};

} // namespace d2