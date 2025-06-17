#pragma once

#include "ui/ui_element.h"
#include <vector>
#include <memory>

namespace d2 {

class UIPanel : public UIElement {
public:
    UIPanel() = default;
    virtual ~UIPanel() = default;
    
    // Child management
    void addChild(std::shared_ptr<UIElement> child) {
        children_.push_back(child);
    }
    
    size_t getChildCount() const { return children_.size(); }
    
    std::shared_ptr<UIElement> getChild(size_t index) const {
        if (index < children_.size()) {
            return children_[index];
        }
        return nullptr;
    }
    
private:
    std::vector<std::shared_ptr<UIElement>> children_;
};

} // namespace d2