#pragma once

namespace d2 {

namespace rendering {
class Renderer;
class SpriteRenderer;
}

class TextRenderer;
class FontManager;
class UIElement;

class UIRenderer {
public:
    UIRenderer() = default;
    ~UIRenderer() = default;
    
    bool initialize(rendering::Renderer* renderer, 
                   rendering::SpriteRenderer* sprite_renderer,
                   TextRenderer* text_renderer,
                   FontManager* font_manager);
    
    bool isInitialized() const;
    
    void beginFrame();
    void renderElement(UIElement* element);
    void endFrame();
    
private:
    bool initialized_ = false;
    rendering::SpriteRenderer* sprite_renderer_ = nullptr;
};

} // namespace d2