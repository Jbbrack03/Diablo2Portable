#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_element.h"
#include "ui/touch_input.h"

namespace d2 {

class UIElementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(UIElementTest, CreateBasicElement) {
    UIElement element;
    
    // Default element should have zero position and size
    EXPECT_EQ(element.getPosition(), glm::vec2(0.0f, 0.0f));
    EXPECT_EQ(element.getSize(), glm::vec2(0.0f, 0.0f));
    EXPECT_TRUE(element.isVisible());
    EXPECT_TRUE(element.isEnabled());
    EXPECT_FALSE(element.isFocused());
}

TEST_F(UIElementTest, SetPositionAndSize) {
    UIElement element;
    
    element.setPosition(glm::vec2(100.0f, 200.0f));
    element.setSize(glm::vec2(300.0f, 400.0f));
    
    EXPECT_EQ(element.getPosition(), glm::vec2(100.0f, 200.0f));
    EXPECT_EQ(element.getSize(), glm::vec2(300.0f, 400.0f));
}

TEST_F(UIElementTest, FocusAndNavigation) {
    UIElement element;
    
    // Initially not focused
    EXPECT_FALSE(element.isFocused());
    
    // Can set focus
    element.setFocused(true);
    EXPECT_TRUE(element.isFocused());
    
    // Can clear focus
    element.setFocused(false);
    EXPECT_FALSE(element.isFocused());
}

TEST_F(UIElementTest, HandleTouchInput) {
    UIElement element;
    element.setPosition(glm::vec2(100.0f, 100.0f));
    element.setSize(glm::vec2(200.0f, 100.0f));
    
    // Touch within bounds should return true
    bool handled = element.handleTouchInput(150.0f, 150.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    
    // Touch outside bounds should return false
    handled = element.handleTouchInput(50.0f, 50.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_FALSE(handled);
    
    // Touch on exact boundary should return true
    handled = element.handleTouchInput(100.0f, 100.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    
    // Touch at far edge should return true
    handled = element.handleTouchInput(299.0f, 199.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_TRUE(handled);
    
    // Touch just outside far edge should return false
    handled = element.handleTouchInput(300.0f, 200.0f, TouchEventType::TOUCH_DOWN);
    EXPECT_FALSE(handled);
}

TEST_F(UIElementTest, BackgroundColorStyling) {
    UIElement element;
    
    // Default should have no background color (transparent)
    EXPECT_EQ(element.getBackgroundColor(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // Should be able to set background color
    glm::vec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);
    element.setBackgroundColor(redColor);
    EXPECT_EQ(element.getBackgroundColor(), redColor);
    
    // Should be able to set semi-transparent color
    glm::vec4 semiTransparent(0.0f, 0.0f, 1.0f, 0.5f);
    element.setBackgroundColor(semiTransparent);
    EXPECT_EQ(element.getBackgroundColor(), semiTransparent);
}

TEST_F(UIElementTest, BackgroundSpriteStyling) {
    UIElement element;
    
    // Default should have no background sprite (0 = no texture)
    EXPECT_EQ(element.getBackgroundSprite(), 0u);
    
    // Should be able to set background sprite ID
    element.setBackgroundSprite(12345); // texture ID
    EXPECT_EQ(element.getBackgroundSprite(), 12345u);
    
    // Should be able to clear background sprite
    element.setBackgroundSprite(0);
    EXPECT_EQ(element.getBackgroundSprite(), 0u);
}

TEST_F(UIElementTest, BorderStyling) {
    UIElement element;
    
    // Default should have no border
    EXPECT_EQ(element.getBorderWidth(), 0.0f);
    EXPECT_EQ(element.getBorderColor(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // Should be able to set border width and color
    element.setBorderWidth(2.0f);
    element.setBorderColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // White border
    
    EXPECT_EQ(element.getBorderWidth(), 2.0f);
    EXPECT_EQ(element.getBorderColor(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Should be able to disable border by setting width to 0
    element.setBorderWidth(0.0f);
    EXPECT_EQ(element.getBorderWidth(), 0.0f);
}

} // namespace d2