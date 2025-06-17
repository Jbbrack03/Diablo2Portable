#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_element.h"

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

} // namespace d2