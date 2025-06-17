#pragma once

namespace d2 {

namespace rendering {
class Renderer;
class SpriteRenderer;
}

class TextRenderer;
class FontManager;

class UIRenderer {
public:
    UIRenderer() = default;
    ~UIRenderer() = default;
    
    bool initialize(rendering::Renderer* renderer, 
                   rendering::SpriteRenderer* sprite_renderer,
                   TextRenderer* text_renderer,
                   FontManager* font_manager);
    
    bool isInitialized() const;
    
private:
    bool initialized_ = false;
};

} // namespace d2