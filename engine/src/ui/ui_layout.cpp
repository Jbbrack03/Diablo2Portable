#include "ui/ui_layout.h"
#include "ui/ui_element.h"

namespace d2 {

void UILayout::setAnchor(UIElement* element, UIAnchor anchor, const glm::vec2& offset) {
    if (!element) return;
    
    anchors_[element] = {anchor, offset};
}

void UILayout::setRelativeSize(UIElement* element, const glm::vec2& relativeSize) {
    if (!element) return;
    
    relativeSizes_[element] = relativeSize;
}

void UILayout::layoutElement(UIElement* element, UIElement* parent) {
    if (!element || !parent) return;
    
    // Apply relative sizing first if set
    auto sizeIt = relativeSizes_.find(element);
    if (sizeIt != relativeSizes_.end()) {
        glm::vec2 parentSize = parent->getSize();
        glm::vec2 newSize = parentSize * sizeIt->second;
        element->setSize(newSize);
    }
    
    auto it = anchors_.find(element);
    if (it == anchors_.end()) return;
    
    const AnchorInfo& info = it->second;
    glm::vec2 parentPos = parent->getPosition();
    glm::vec2 parentSize = parent->getSize();
    glm::vec2 elementSize = element->getSize();
    glm::vec2 newPos = parentPos;
    
    switch (info.anchor) {
        case UIAnchor::TOP_LEFT:
            newPos = parentPos + info.offset;
            break;
            
        case UIAnchor::TOP_CENTER:
            newPos.x = parentPos.x + parentSize.x / 2 - elementSize.x / 2 + info.offset.x;
            newPos.y = parentPos.y + info.offset.y;
            break;
            
        case UIAnchor::TOP_RIGHT:
            newPos.x = parentPos.x + parentSize.x - elementSize.x + info.offset.x;
            newPos.y = parentPos.y + info.offset.y;
            break;
            
        case UIAnchor::CENTER_LEFT:
            newPos.x = parentPos.x + info.offset.x;
            newPos.y = parentPos.y + parentSize.y / 2 - elementSize.y / 2 + info.offset.y;
            break;
            
        case UIAnchor::CENTER:
            newPos.x = parentPos.x + parentSize.x / 2 - elementSize.x / 2 + info.offset.x;
            newPos.y = parentPos.y + parentSize.y / 2 - elementSize.y / 2 + info.offset.y;
            break;
            
        case UIAnchor::CENTER_RIGHT:
            newPos.x = parentPos.x + parentSize.x - elementSize.x + info.offset.x;
            newPos.y = parentPos.y + parentSize.y / 2 - elementSize.y / 2 + info.offset.y;
            break;
            
        case UIAnchor::BOTTOM_LEFT:
            newPos.x = parentPos.x + info.offset.x;
            newPos.y = parentPos.y + parentSize.y - elementSize.y + info.offset.y;
            break;
            
        case UIAnchor::BOTTOM_CENTER:
            newPos.x = parentPos.x + parentSize.x / 2 - elementSize.x / 2 + info.offset.x;
            newPos.y = parentPos.y + parentSize.y - elementSize.y + info.offset.y;
            break;
            
        case UIAnchor::BOTTOM_RIGHT:
            newPos.x = parentPos.x + parentSize.x - elementSize.x + info.offset.x;
            newPos.y = parentPos.y + parentSize.y - elementSize.y + info.offset.y;
            break;
    }
    
    element->setPosition(newPos);
}

} // namespace d2