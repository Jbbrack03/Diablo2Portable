#include "ui/ui_renderer.h"
#include "rendering/renderer.h"
#include "rendering/sprite_renderer.h"
#include "ui/text_renderer.h"
#include "ui/font_manager.h"
#include "ui/ui_element.h"
#include "ui/ui_label.h"
#include "ui/font.h"

namespace d2 {

bool UIRenderer::initialize(rendering::Renderer* renderer, 
                           rendering::SpriteRenderer* sprite_renderer,
                           TextRenderer* text_renderer,
                           FontManager* font_manager) {
    if (!renderer || !sprite_renderer || !text_renderer || !font_manager) {
        return false;
    }
    
    sprite_renderer_ = sprite_renderer;
    text_renderer_ = text_renderer;
    initialized_ = true;
    return true;
}

bool UIRenderer::isInitialized() const {
    return initialized_;
}

void UIRenderer::beginFrame() {
    // Reset debug counters
    background_draw_count_ = 0;
    border_draw_count_ = 0;
    
    if (sprite_renderer_) {
        sprite_renderer_->beginFrame();
    }
}

void UIRenderer::renderElement(UIElement* element) {
    if (!element || !sprite_renderer_ || !element->isVisible()) {
        return;
    }
    
    glm::vec2 pos = element->getPosition();
    glm::vec2 size = element->getSize();
    
    // Render background color if set
    glm::vec4 bgColor = element->getBackgroundColor();
    if (bgColor.a > 0.0f) {
        // In a real implementation, this would draw a colored quad
        // For now, we'll just track that we drew it
        background_draw_count_++;
    }
    
    // Render background sprite if set
    uint32_t bgSprite = element->getBackgroundSprite();
    if (bgSprite != 0) {
        sprite_renderer_->drawSprite(bgSprite, pos, size);
        background_draw_count_++;
    }
    
    // Render border if set
    float borderWidth = element->getBorderWidth();
    if (borderWidth > 0.0f) {
        glm::vec4 borderColor = element->getBorderColor();
        if (borderColor.a > 0.0f) {
            // In a real implementation, this would draw border lines/quads
            // For now, we'll just track that we drew it
            border_draw_count_++;
        }
    }
    
    // Check if this is a UILabel
    if (auto* label = dynamic_cast<UILabel*>(element)) {
        // Render text
        if (text_renderer_ && default_font_ && !label->getText().empty()) {
            text_renderer_->renderText(label->getText(), label->getPosition(), default_font_);
        }
    } else {
        // For non-label elements without styling, render a default sprite
        if (bgColor.a == 0.0f && bgSprite == 0 && borderWidth == 0.0f) {
            sprite_renderer_->drawSprite(0, pos, size);
        }
    }
}

void UIRenderer::endFrame() {
    if (sprite_renderer_) {
        sprite_renderer_->endFrame();
    }
}

void UIRenderer::setDefaultFont(Font* font) {
    default_font_ = font;
}

} // namespace d2