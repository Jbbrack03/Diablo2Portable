#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_panel.h"
#include "ui/ui_button.h"
#include "ui/touch_input.h"

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

TEST_F(UIPanelTest, TouchInputPropagation) {
    UIPanel panel;
    panel.setPosition(glm::vec2(0.0f, 0.0f));
    panel.setSize(glm::vec2(400.0f, 300.0f));
    
    // Add buttons to panel at different positions
    auto button1 = std::make_shared<UIButton>("Button 1");
    button1->setPosition(glm::vec2(10.0f, 10.0f));  // Relative to panel
    button1->setSize(glm::vec2(100.0f, 50.0f));
    
    auto button2 = std::make_shared<UIButton>("Button 2");
    button2->setPosition(glm::vec2(120.0f, 10.0f));  // Relative to panel
    button2->setSize(glm::vec2(100.0f, 50.0f));
    
    // Track button clicks
    bool button1_clicked = false;
    bool button2_clicked = false;
    button1->setOnClick([&button1_clicked]() { button1_clicked = true; });
    button2->setOnClick([&button2_clicked]() { button2_clicked = true; });
    
    panel.addChild(button1);
    panel.addChild(button2);
    
    // Touch on button 1 (absolute coordinates)
    bool handled = panel.handleTouchInput(10.0f, 10.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    
    handled = panel.handleTouchInput(10.0f, 10.0f, TouchEventType::TOUCH_UP);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(button1_clicked);
    EXPECT_FALSE(button2_clicked);
    
    // Reset
    button1_clicked = false;
    
    // Touch on button 2 (absolute coordinates)
    handled = panel.handleTouchInput(120.0f, 10.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    
    handled = panel.handleTouchInput(120.0f, 10.0f, TouchEventType::TOUCH_UP);
    EXPECT_TRUE(handled);
    EXPECT_FALSE(button1_clicked);
    EXPECT_TRUE(button2_clicked);
    
    // Touch outside any button but inside panel
    handled = panel.handleTouchInput(250.0f, 150.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);  // Panel handles it
    
    // Touch outside panel
    handled = panel.handleTouchInput(500.0f, 500.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_FALSE(handled);
}

} // namespace d2