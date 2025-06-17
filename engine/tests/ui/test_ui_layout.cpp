#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_layout.h"
#include "ui/ui_element.h"
#include "ui/ui_button.h"
#include "ui/ui_panel.h"

namespace d2::test {

class UILayoutTest : public ::testing::Test {
protected:
    void SetUp() override {
        layout_ = std::make_unique<UILayout>();
    }
    
    std::unique_ptr<UILayout> layout_;
};

// Test 1: Basic anchoring to parent edges
TEST_F(UILayoutTest, AnchorToParentEdges) {
    // Create a parent container
    auto parent = std::make_unique<UIPanel>();
    parent->setPosition(glm::vec2(100, 100));
    parent->setSize(glm::vec2(400, 300));
    
    // Create a child element
    auto child = std::make_unique<UIElement>();
    child->setSize(glm::vec2(100, 50));
    
    // Anchor child to top-left of parent with 10px margin
    layout_->setAnchor(child.get(), UIAnchor::TOP_LEFT, glm::vec2(10, 10));
    layout_->layoutElement(child.get(), parent.get());
    
    // Verify child position
    EXPECT_EQ(child->getPosition(), glm::vec2(110, 110)); // parent pos + margin
    
    // Anchor child to bottom-right of parent with 10px margin
    layout_->setAnchor(child.get(), UIAnchor::BOTTOM_RIGHT, glm::vec2(-10, -10));
    layout_->layoutElement(child.get(), parent.get());
    
    // Verify child position (parent bottom-right - child size - margin)
    EXPECT_EQ(child->getPosition(), glm::vec2(390, 340)); // (100+400-100-10, 100+300-50-10)
}

// Test 2: Relative sizing based on parent
TEST_F(UILayoutTest, RelativeSizingToParent) {
    // Create a parent container
    auto parent = std::make_unique<UIPanel>();
    parent->setPosition(glm::vec2(0, 0));
    parent->setSize(glm::vec2(800, 600));
    
    // Create a child element
    auto child = std::make_unique<UIElement>();
    
    // Set child to be 50% width and 25% height of parent
    layout_->setRelativeSize(child.get(), glm::vec2(0.5f, 0.25f));
    layout_->layoutElement(child.get(), parent.get());
    
    // Verify child size
    EXPECT_EQ(child->getSize(), glm::vec2(400, 150)); // 50% of 800, 25% of 600
    
    // Change parent size
    parent->setSize(glm::vec2(1000, 800));
    layout_->layoutElement(child.get(), parent.get());
    
    // Verify child size updates
    EXPECT_EQ(child->getSize(), glm::vec2(500, 200)); // 50% of 1000, 25% of 800
}

} // namespace d2::test