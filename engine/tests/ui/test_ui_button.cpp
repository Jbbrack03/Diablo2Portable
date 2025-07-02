#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_button.h"
#include "ui/touch_input.h"
#include "ui/ui_manager.h"

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

TEST_F(UIButtonTest, VisualStateSprites) {
    UIButton button("Visual Button");
    
    // Default should have no state sprites
    EXPECT_EQ(button.getNormalSprite(), 0u);
    EXPECT_EQ(button.getHoverSprite(), 0u);
    EXPECT_EQ(button.getPressedSprite(), 0u);
    EXPECT_EQ(button.getDisabledSprite(), 0u);
    
    // Should be able to set state sprites
    button.setNormalSprite(100);
    button.setHoverSprite(101);
    button.setPressedSprite(102);
    button.setDisabledSprite(103);
    
    EXPECT_EQ(button.getNormalSprite(), 100u);
    EXPECT_EQ(button.getHoverSprite(), 101u);
    EXPECT_EQ(button.getPressedSprite(), 102u);
    EXPECT_EQ(button.getDisabledSprite(), 103u);
    
    // Should return correct sprite based on state
    EXPECT_EQ(button.getCurrentSprite(), 100u); // Normal state
    
    // Hover state
    button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_MOVE);
    button.setPosition(glm::vec2(100.0f, 100.0f));
    button.setSize(glm::vec2(200.0f, 50.0f));
    button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_MOVE);
    EXPECT_EQ(button.getCurrentSprite(), 101u); // Hover sprite
    
    // Pressed state
    button.handleTouchInput(150.0f, 125.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_EQ(button.getCurrentSprite(), 102u); // Pressed sprite
}

TEST_F(UIButtonTest, HandleControllerInput) {
    UIButton button("Controller Button");
    
    // Click callback tracking
    bool clicked = false;
    button.setOnClick([&clicked]() { clicked = true; });
    
    // Button should be focusable for controller input
    button.setFocused(true);
    EXPECT_TRUE(button.isFocused());
    
    // Action button should trigger click when focused
    bool handled = button.handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(clicked);
}

TEST_F(UIButtonTest, ControllerNavigationStates) {
    UIButton button("Nav Button");
    button.setNormalSprite(100);
    button.setHoverSprite(200);
    button.setPressedSprite(201);
    
    // Normal state initially
    EXPECT_EQ(button.getCurrentSprite(), 100u); // Normal sprite
    
    // Focus should trigger hover sprite
    button.setFocused(true);
    EXPECT_EQ(button.getCurrentSprite(), 200u); // Hover sprite when focused
    
    // Verify controller action is handled when focused
    bool clicked = false;
    button.setOnClick([&clicked]() { clicked = true; });
    
    bool handled = button.handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_TRUE(clicked);
}

} // namespace d2