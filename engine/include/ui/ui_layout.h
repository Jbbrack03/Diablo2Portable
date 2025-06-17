#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

namespace d2 {

class UIElement;

enum class UIAnchor {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};

class UILayout {
public:
    UILayout() = default;
    ~UILayout() = default;
    
    void setAnchor(UIElement* element, UIAnchor anchor, const glm::vec2& offset = glm::vec2(0, 0));
    void layoutElement(UIElement* element, UIElement* parent);
    
private:
    struct AnchorInfo {
        UIAnchor anchor;
        glm::vec2 offset;
    };
    
    std::unordered_map<UIElement*, AnchorInfo> anchors_;
};

} // namespace d2