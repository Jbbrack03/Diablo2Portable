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

} // namespace d2