#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_button.h"
#include "ui/touch_input.h"

namespace d2 {

class UIButtonTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(UIButtonTest, CreateBasicButton) {
    UIButton button("Test Button");
    
    // Button should have text
    EXPECT_EQ(button.getText(), "Test Button");
    
    // Button should inherit from UIElement
    EXPECT_EQ(button.getPosition(), glm::vec2(0.0f, 0.0f));
    EXPECT_TRUE(button.isVisible());
    EXPECT_TRUE(button.isEnabled());
    
    // Button-specific state
    EXPECT_FALSE(button.isPressed());
    EXPECT_FALSE(button.isHovered());
}

TEST_F(UIButtonTest, HandleClickEvents) {
    UIButton button("Click Me");
    button.setPosition(glm::vec2(100.0f, 100.0f));
    button.setSize(glm::vec2(200.0f, 50.0f));
    
    // Click callback tracking
    bool clicked = false;
    button.setOnClick([&clicked]() { clicked = true; });
    
    // Click inside button bounds
    button.handleMouseDown(glm::vec2(150.0f, 125.0f));
    EXPECT_TRUE(button.isPressed());
    
    button.handleMouseUp(glm::vec2(150.0f, 125.0f));
    EXPECT_FALSE(button.isPressed());
    EXPECT_TRUE(clicked);
}

TEST_F(UIButtonTest, HandleTouchInput) {
    UIButton button("Touch Me");
    button.setPosition(glm::vec2(100.0f, 100.0f));
    button.setSize(glm::vec2(200.0f, 50.0f));
    
    // Set up click callback
    bool clicked = false;
    button.setOnClick([&clicked]() { clicked = true; });
    
    // Touch down should set pressed state
    bool handled = button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(button.isPressed());
    EXPECT_FALSE(clicked); // Not clicked until release
    
    // Touch up at same location should trigger click
    handled = button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_UP);
    EXPECT_TRUE(handled);
    EXPECT_FALSE(button.isPressed());
    EXPECT_TRUE(clicked);
    
    // Reset click state
    clicked = false;
    
    // Touch down then move out then up should not trigger click
    handled = button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(button.isPressed());
    
    // Touch up outside bounds should not trigger click
    handled = button.handleTouchInput(50.0f, 50.0f, TouchEventType::TOUCH_UP);
    EXPECT_FALSE(handled); // Outside bounds
    EXPECT_FALSE(button.isPressed()); // Should still clear pressed state
    EXPECT_FALSE(clicked); // Should not trigger click
    
    // Touch move should update hover state
    handled = button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_MOVE);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(button.isHovered());
    
    // Touch move outside should clear hover
    handled = button.handleTouchInput(50.0f, 50.0f, TouchEventType::TOUCH_MOVE);
    EXPECT_FALSE(handled);
    EXPECT_FALSE(button.isHovered());
}

} // namespace d2