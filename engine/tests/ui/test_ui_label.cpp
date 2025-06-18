#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>
#include "ui/ui_label.h"
#include "ui/font.h"
#include "ui/text_renderer.h"

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

TEST_F(UILabelTest, TextAlignment) {
    UILabel label("Aligned text");
    
    // Default alignment should be left
    EXPECT_EQ(label.getAlignment(), UILabel::Alignment::LEFT);
    
    // Should be able to set center alignment
    label.setAlignment(UILabel::Alignment::CENTER);
    EXPECT_EQ(label.getAlignment(), UILabel::Alignment::CENTER);
    
    // Should be able to set right alignment
    label.setAlignment(UILabel::Alignment::RIGHT);
    EXPECT_EQ(label.getAlignment(), UILabel::Alignment::RIGHT);
}

TEST_F(UILabelTest, FontIntegration) {
    UILabel label("Test Text");
    
    // Create a font for testing
    auto font = std::make_shared<Font>("Arial", 16);
    std::vector<uint8_t> atlasData(256 * 256, 255);
    font->loadFromAtlasData(atlasData, 256, 256);
    
    // Should be able to set and get font
    label.setFont(font);
    EXPECT_EQ(label.getFont(), font);
    
    // Font should affect text width calculation
    int widthWithFont = label.getTextWidth();
    EXPECT_GT(widthWithFont, 0);
    
    // Different fonts should give different widths
    auto largerFont = std::make_shared<Font>("Arial", 24);
    largerFont->loadFromAtlasData(atlasData, 256, 256);
    label.setFont(largerFont);
    
    int widthWithLargerFont = label.getTextWidth();
    EXPECT_GT(widthWithLargerFont, widthWithFont);
}

TEST_F(UILabelTest, RenderWithTextRenderer) {
    UILabel label("Render Test");
    label.setPosition(glm::vec2(100, 100));
    
    // Create font and text renderer
    auto font = std::make_shared<Font>("Arial", 16);
    std::vector<uint8_t> atlasData(256 * 256, 255);
    font->loadFromAtlasData(atlasData, 256, 256);
    label.setFont(font);
    
    TextRenderer textRenderer;
    textRenderer.initialize();
    
    // Should be able to render without crashing
    EXPECT_NO_THROW(label.render(textRenderer));
    
    // Text renderer should have generated vertices
    EXPECT_GT(textRenderer.getVertexCount(), 0);
}

} // namespace d2