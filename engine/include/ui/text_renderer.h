#pragma once

#include <glm/glm.hpp>

namespace d2 {

class TextRenderer {
public:
    TextRenderer() = default;
    virtual ~TextRenderer() = default;
    
    const glm::vec4& getColor() const { return color_; }
    
private:
    glm::vec4 color_{1.0f, 1.0f, 1.0f, 1.0f};  // Default white
};

} // namespace d2