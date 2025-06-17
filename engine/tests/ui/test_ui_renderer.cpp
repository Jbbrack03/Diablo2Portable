#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "ui/ui_renderer.h"
#include "ui/ui_element.h"
#include "ui/ui_label.h"
#include "ui/ui_button.h"
#include "ui/ui_panel.h"
#include "rendering/renderer.h"
#include "rendering/sprite_renderer.h"
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
    MockSpriteRenderer() { initialized_ = true; }
    bool isInitialized() const { return initialized_; }
    void beginFrame() { frame_begun_ = true; }
    void endFrame() { frame_ended_ = true; }
    void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size) {
        last_texture_id_ = texture_id;
        last_position_ = position;
        last_size_ = size;
        sprite_count_++;
    }
    
    bool frame_begun_ = false;
    bool frame_ended_ = false;
    uint32_t last_texture_id_ = 0;
    glm::vec2 last_position_{0, 0};
    glm::vec2 last_size_{0, 0};
    uint32_t sprite_count_ = 0;
private:
    bool initialized_ = false;
};

class MockTextRenderer : public TextRenderer {
public:
    void renderText(const std::string& text, const glm::vec2& position, const Font* font) {
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

} // namespace d2::test