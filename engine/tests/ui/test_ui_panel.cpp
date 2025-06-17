#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_panel.h"
#include "ui/ui_button.h"

namespace d2 {

class UIPanelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(UIPanelTest, CreateEmptyPanel) {
    UIPanel panel;
    
    // Panel should start empty
    EXPECT_EQ(panel.getChildCount(), 0);
    
    // Panel should inherit from UIElement
    EXPECT_EQ(panel.getPosition(), glm::vec2(0.0f, 0.0f));
    EXPECT_TRUE(panel.isVisible());
    EXPECT_TRUE(panel.isEnabled());
}

TEST_F(UIPanelTest, AddChildElements) {
    UIPanel panel;
    
    // Add buttons to panel
    auto button1 = std::make_shared<UIButton>("Button 1");
    auto button2 = std::make_shared<UIButton>("Button 2");
    
    panel.addChild(button1);
    panel.addChild(button2);
    
    EXPECT_EQ(panel.getChildCount(), 2);
    EXPECT_EQ(panel.getChild(0), button1);
    EXPECT_EQ(panel.getChild(1), button2);
}

TEST_F(UIPanelTest, FocusManagement) {
    UIPanel panel;
    
    auto button1 = std::make_shared<UIButton>("Button 1");
    auto button2 = std::make_shared<UIButton>("Button 2");
    auto button3 = std::make_shared<UIButton>("Button 3");
    
    panel.addChild(button1);
    panel.addChild(button2);
    panel.addChild(button3);
    
    // Initially no focus
    EXPECT_EQ(panel.getFocusedChildIndex(), -1);
    
    // Focus first child
    panel.focusChild(0);
    EXPECT_EQ(panel.getFocusedChildIndex(), 0);
    EXPECT_TRUE(button1->isFocused());
    EXPECT_FALSE(button2->isFocused());
    
    // Focus next child
    panel.focusNext();
    EXPECT_EQ(panel.getFocusedChildIndex(), 1);
    EXPECT_FALSE(button1->isFocused());
    EXPECT_TRUE(button2->isFocused());
    
    // Focus previous child
    panel.focusPrevious();
    EXPECT_EQ(panel.getFocusedChildIndex(), 0);
    EXPECT_TRUE(button1->isFocused());
    EXPECT_FALSE(button2->isFocused());
}

} // namespace d2