#pragma once

#include <glm/glm.hpp>
#include <string>

namespace d2 {

class Font;

class TextRenderer {
public:
    TextRenderer() = default;
    virtual ~TextRenderer() = default;
    
    const glm::vec4& getColor() const { return color_; }
    void setColor(const glm::vec4& color) { color_ = color; }
    
    virtual void renderText(const std::string& text, const glm::vec2& position, const Font* font) {
        // Base implementation does nothing
        // Derived classes should override this
    }
    
private:
    glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};  // Default white
};

} // namespace d2