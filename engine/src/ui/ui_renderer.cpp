#include "ui/ui_renderer.h"
#include "rendering/renderer.h"
#include "rendering/sprite_renderer.h"
#include "ui/text_renderer.h"
#include "ui/font_manager.h"
#include "ui/ui_element.h"

namespace d2 {

bool UIRenderer::initialize(rendering::Renderer* renderer, 
                           rendering::SpriteRenderer* sprite_renderer,
                           TextRenderer* text_renderer,
                           FontManager* font_manager) {
    if (!renderer || !sprite_renderer || !text_renderer || !font_manager) {
        return false;
    }
    
    sprite_renderer_ = sprite_renderer;
    initialized_ = true;
    return true;
}

bool UIRenderer::isInitialized() const {
    return initialized_;
}

void UIRenderer::beginFrame() {
    if (sprite_renderer_) {
        sprite_renderer_->beginFrame();
    }
}

void UIRenderer::renderElement(UIElement* element) {
    if (!element || !sprite_renderer_ || !element->isVisible()) {
        return;
    }
    
    // For now, render a simple sprite at the element's position with its size
    // In a real implementation, this would be more sophisticated
    sprite_renderer_->drawSprite(0, element->getPosition(), element->getSize());
}

void UIRenderer::endFrame() {
    if (sprite_renderer_) {
        sprite_renderer_->endFrame();
    }
}

} // namespace d2