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
    
    int getTextWidth(const std::string& text) const {
        if (text.empty()) return 0;
        
        // Simple placeholder implementation
        // In real implementation, this would use font metrics
        // For now, approximate each character as size/2 wide
        return static_cast<int>(text.length() * size_ / 2);
    }
    
private:
    std::string name_;
    int size_;
    int lineHeight_;
};

} // namespace d2