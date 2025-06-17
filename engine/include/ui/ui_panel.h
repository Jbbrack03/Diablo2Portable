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
    
    // Focus management
    int getFocusedChildIndex() const { return focusedChildIndex_; }
    
    void focusChild(int index) {
        if (focusedChildIndex_ >= 0 && focusedChildIndex_ < static_cast<int>(children_.size())) {
            children_[focusedChildIndex_]->setFocused(false);
        }
        
        if (index >= 0 && index < static_cast<int>(children_.size())) {
            focusedChildIndex_ = index;
            children_[index]->setFocused(true);
        } else {
            focusedChildIndex_ = -1;
        }
    }
    
    void focusNext() {
        if (children_.empty()) return;
        
        int nextIndex = focusedChildIndex_ + 1;
        if (nextIndex >= static_cast<int>(children_.size())) {
            nextIndex = 0;  // Wrap around
        }
        focusChild(nextIndex);
    }
    
    void focusPrevious() {
        if (children_.empty()) return;
        
        int prevIndex = focusedChildIndex_ - 1;
        if (prevIndex < 0) {
            prevIndex = static_cast<int>(children_.size()) - 1;  // Wrap around
        }
        focusChild(prevIndex);
    }
    
private:
    std::vector<std::shared_ptr<UIElement>> children_;
    int focusedChildIndex_{-1};
};

} // namespace d2