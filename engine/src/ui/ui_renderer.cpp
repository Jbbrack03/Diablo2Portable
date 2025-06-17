#include "ui/ui_renderer.h"
#include "rendering/renderer.h"
#include "rendering/sprite_renderer.h"
#include "ui/text_renderer.h"
#include "ui/font_manager.h"

namespace d2 {

bool UIRenderer::initialize(rendering::Renderer* renderer, 
                           rendering::SpriteRenderer* sprite_renderer,
                           TextRenderer* text_renderer,
                           FontManager* font_manager) {
    if (!renderer || !sprite_renderer || !text_renderer || !font_manager) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool UIRenderer::isInitialized() const {
    return initialized_;
}

} // namespace d2