#pragma once

#include <string>

namespace d2 {

class Font {
public:
    Font(const std::string& name, int size) : name_(name), size_(size) {
        // Line height is typically 120% of font size
        lineHeight_ = static_cast<int>(size * 1.2f);
    }
    virtual ~Font() = default;
    
    const std::string& getName() const { return name_; }
    int getSize() const { return size_; }
    int getLineHeight() const { return lineHeight_; }
    
private:
    std::string name_;
    int size_;
    int lineHeight_;
};

} // namespace d2