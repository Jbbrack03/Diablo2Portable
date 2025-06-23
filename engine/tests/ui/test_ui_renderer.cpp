#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_renderer.h"
#include "ui/ui_element.h"
#include "ui/ui_label.h"
#include "ui/ui_button.h"
#include "ui/ui_panel.h"
#include "rendering/renderer.h"
#include "rendering/sprite_renderer.h"
#include "rendering/texture_manager.h"
#include "ui/text_renderer.h"
#include "ui/font_manager.h"

namespace d2::test {

// Mock classes for testing
class MockRenderer : public rendering::Renderer {
public:
    MockRenderer() { initialized_ = true; }
    bool isInitialized() const { return initialized_; }
private:
    bool initialized_ = false;
};

class MockSpriteRenderer : public rendering::SpriteRenderer {
public:
    MockSpriteRenderer() = default;
};

class MockTextRenderer : public TextRenderer {
public:
    void renderText(const std::string& text, const glm::vec2& position, const Font* font) override {
        last_text_ = text;
        last_position_ = position;
        last_font_ = font;
        render_count_++;
    }
    
    std::string last_text_;
    glm::vec2 last_position_{0, 0};
    const Font* last_font_ = nullptr;
    int render_count_ = 0;
};

class UIRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        renderer_ = std::make_unique<MockRenderer>();
        sprite_renderer_ = std::make_unique<MockSpriteRenderer>();
        text_renderer_ = std::make_unique<MockTextRenderer>();
        font_manager_ = std::make_unique<FontManager>();
        ui_renderer_ = std::make_unique<UIRenderer>();
    }
    
    std::unique_ptr<MockRenderer> renderer_;
    std::unique_ptr<MockSpriteRenderer> sprite_renderer_;
    std::unique_ptr<MockTextRenderer> text_renderer_;
    std::unique_ptr<FontManager> font_manager_;
    std::unique_ptr<UIRenderer> ui_renderer_;
};

// Test 1: UIRenderer initialization
TEST_F(UIRendererTest, InitializationRequiresValidComponents) {
    // Should fail with null renderer
    EXPECT_FALSE(ui_renderer_->initialize(nullptr, sprite_renderer_.get(), text_renderer_.get(), font_manager_.get()));
    
    // Should fail with null sprite renderer
    EXPECT_FALSE(ui_renderer_->initialize(renderer_.get(), nullptr, text_renderer_.get(), font_manager_.get()));
    
    // Should fail with null text renderer
    EXPECT_FALSE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), nullptr, font_manager_.get()));
    
    // Should fail with null font manager
    EXPECT_FALSE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), text_renderer_.get(), nullptr));
    
    // Should succeed with all valid components
    EXPECT_TRUE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), text_renderer_.get(), font_manager_.get()));
    EXPECT_TRUE(ui_renderer_->isInitialized());
}

// Test 2: Render a single UIElement
TEST_F(UIRendererTest, RenderSingleUIElement) {
    // Initialize the UI renderer
    ASSERT_TRUE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), text_renderer_.get(), font_manager_.get()));
    
    // Create a simple UI element
    auto element = std::make_unique<UIElement>();
    element->setPosition(glm::vec2(100, 200));
    element->setSize(glm::vec2(50, 30));
    // Element is visible by default
    
    // Get initial sprite count
    uint32_t initial_sprite_count = sprite_renderer_->getSpriteCount();
    
    // Render the element
    ui_renderer_->beginFrame();
    ui_renderer_->renderElement(element.get());
    ui_renderer_->endFrame();
    
    // Verify that a sprite was rendered (sprite count should increase)
    EXPECT_EQ(sprite_renderer_->getSpriteCount(), initial_sprite_count + 1);
}

// Test 3: Render a UILabel with text
TEST_F(UIRendererTest, RenderUILabelWithText) {
    // Initialize the UI renderer
    ASSERT_TRUE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), text_renderer_.get(), font_manager_.get()));
    
    // Create a font and set it as default
    auto font = font_manager_->createFont("Arial", 16);
    ASSERT_NE(font, nullptr);
    ui_renderer_->setDefaultFont(font.get());
    
    // Create a UILabel
    auto label = std::make_unique<UILabel>("Hello World");
    label->setPosition(glm::vec2(50, 100));
    label->setSize(glm::vec2(200, 30));
    
    // Render the label
    ui_renderer_->beginFrame();
    ui_renderer_->renderElement(label.get());
    ui_renderer_->endFrame();
    
    // Verify that text was rendered
    EXPECT_EQ(text_renderer_->render_count_, 1);
    EXPECT_EQ(text_renderer_->last_text_, "Hello World");
    EXPECT_EQ(text_renderer_->last_position_, glm::vec2(50, 100));
    EXPECT_EQ(text_renderer_->last_font_, font.get());
}

TEST_F(UIRendererTest, RenderStyledElements) {
    // Initialize renderer
    ASSERT_TRUE(ui_renderer_->initialize(renderer_.get(), sprite_renderer_.get(), 
                                         text_renderer_.get(), font_manager_.get()));
    
    // Create a styled button
    auto button = std::make_unique<UIButton>("Styled Button");
    button->setPosition(glm::vec2(100, 200));
    button->setSize(glm::vec2(150, 50));
    button->setBackgroundColor(glm::vec4(0.2f, 0.3f, 0.8f, 1.0f)); // Blue background
    button->setBorderWidth(2.0f);
    button->setBorderColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // White border
    
    // Mock sprite renderer should track draw calls
    int background_draws = ui_renderer_->getBackgroundDrawCount();
    int border_draws = ui_renderer_->getBorderDrawCount();
    
    ui_renderer_->beginFrame();
    ui_renderer_->renderElement(button.get());
    ui_renderer_->endFrame();
    
    // Verify background and border were rendered
    EXPECT_EQ(ui_renderer_->getBackgroundDrawCount(), background_draws + 1);
    EXPECT_EQ(ui_renderer_->getBorderDrawCount(), border_draws + 1);
}

} // namespace d2::test