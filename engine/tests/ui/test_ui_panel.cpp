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

} // namespace d2