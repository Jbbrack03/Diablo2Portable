#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_button.h"

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

} // namespace d2