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

TEST_F(UILabelTest, SetTextDynamically) {
    UILabel label("Initial text");
    EXPECT_EQ(label.getText(), "Initial text");
    
    // Should be able to change text
    label.setText("Updated text");
    EXPECT_EQ(label.getText(), "Updated text");
    
    // Setting empty text should work
    label.setText("");
    EXPECT_EQ(label.getText(), "");
}

} // namespace d2