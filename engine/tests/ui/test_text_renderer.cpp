#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/text_renderer.h"
#include "ui/font.h"

namespace d2 {

class TextRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        font_ = std::make_shared<Font>("Arial", 16);
    }
    
    std::shared_ptr<Font> font_;
};

TEST_F(TextRendererTest, CreateTextRenderer) {
    TextRenderer textRenderer;
    
    // Should be able to create a text renderer
    // Initially should have default color (white)
    EXPECT_EQ(textRenderer.getColor(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

TEST_F(TextRendererTest, SetTextColor) {
    TextRenderer textRenderer;
    
    // Set red color
    textRenderer.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(textRenderer.getColor(), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // Set semi-transparent blue
    textRenderer.setColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
    EXPECT_EQ(textRenderer.getColor(), glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
}

TEST_F(TextRendererTest, InitializeOpenGLResources) {
    TextRenderer textRenderer;
    
    // TextRenderer should be able to initialize OpenGL resources
    EXPECT_TRUE(textRenderer.initialize());
    
    // After initialization, should have valid shader program and VBO
    EXPECT_GT(textRenderer.getShaderProgram(), 0);
    EXPECT_GT(textRenderer.getVertexBuffer(), 0);
    EXPECT_GT(textRenderer.getVertexArray(), 0);
}

TEST_F(TextRendererTest, RenderTextWithBitmapFont) {
    TextRenderer textRenderer;
    textRenderer.initialize();
    
    // Create bitmap font with atlas data
    std::vector<uint8_t> fontAtlasData(256 * 256, 255);
    font_->loadFromAtlasData(fontAtlasData, 256, 256);
    
    // Should be able to render text without crashing
    EXPECT_NO_THROW(textRenderer.renderText("Hello", glm::vec2(100, 100), font_.get()));
    
    // Verify that vertices were generated for text rendering
    EXPECT_GT(textRenderer.getVertexCount(), 0);
}

TEST_F(TextRendererTest, BatchMultipleTextRenders) {
    TextRenderer textRenderer;
    textRenderer.initialize();
    
    std::vector<uint8_t> fontAtlasData(256 * 256, 255);
    font_->loadFromAtlasData(fontAtlasData, 256, 256);
    
    // Start batching
    textRenderer.beginBatch();
    
    // Add multiple text renders to batch
    textRenderer.renderText("Text1", glm::vec2(0, 0), font_.get());
    textRenderer.renderText("Text2", glm::vec2(0, 20), font_.get());
    textRenderer.renderText("Text3", glm::vec2(0, 40), font_.get());
    
    // End batch should render all text in single draw call
    int drawCalls = textRenderer.endBatch();
    EXPECT_EQ(drawCalls, 1);  // Should batch into single draw call
}

} // namespace d2