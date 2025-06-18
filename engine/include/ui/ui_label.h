#pragma once

#include "ui/ui_element.h"
#include "ui/font.h"
#include "ui/text_renderer.h"
#include <string>
#include <memory>

namespace d2 {

class UILabel : public UIElement {
public:
    enum class Alignment {
        LEFT,
        CENTER,
        RIGHT
    };
    
    explicit UILabel(const std::string& text) : text_(text) {}
    virtual ~UILabel() = default;
    
    const std::string& getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }
    
    Alignment getAlignment() const { return alignment_; }
    void setAlignment(Alignment alignment) { alignment_ = alignment; }
    
    // Font integration
    void setFont(std::shared_ptr<Font> font) { font_ = font; }
    std::shared_ptr<Font> getFont() const { return font_; }
    
    // Text measurement
    int getTextWidth() const {
        if (!font_ || text_.empty()) return 0;
        return font_->getTextWidth(text_);
    }
    
    int getTextHeight() const {
        if (!font_) return 0;
        return font_->getLineHeight();
    }
    
    // Rendering
    void render(TextRenderer& textRenderer) {
        if (!font_ || text_.empty() || !isVisible()) return;
        
        glm::vec2 renderPos = getPosition();
        
        // Adjust position based on alignment
        if (alignment_ == Alignment::CENTER) {
            renderPos.x -= getTextWidth() / 2.0f;
        } else if (alignment_ == Alignment::RIGHT) {
            renderPos.x -= getTextWidth();
        }
        
        textRenderer.renderText(text_, renderPos, font_.get());
    }
    
private:
    std::string text_;
    Alignment alignment_{Alignment::LEFT};
    std::shared_ptr<Font> font_;
};

} // namespace d2