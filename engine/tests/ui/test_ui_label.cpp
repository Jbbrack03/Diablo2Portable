#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_label.h"

namespace d2 {

class UILabelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(UILabelTest, CreateBasicLabel) {
    UILabel label("Hello World");
    
    // Label should have the text set in constructor
    EXPECT_EQ(label.getText(), "Hello World");
    
    // Should inherit from UIElement
    EXPECT_EQ(label.getPosition(), glm::vec2(0.0f, 0.0f));
    EXPECT_TRUE(label.isVisible());
    EXPECT_TRUE(label.isEnabled());
}

} // namespace d2