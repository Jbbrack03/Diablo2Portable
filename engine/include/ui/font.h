#pragma once

#include <string>
#include <vector>

namespace d2 {

struct CharacterInfo {
    int width;
    int height;
    float u;  // UV coordinates for texture atlas
    float v;
    float uWidth;
    float vHeight;
    int xOffset;
    int yOffset;
    int advance;
};

class Font {
public:
    Font(const std::string& name, int size) : name_(name), size_(size), textureId_(0) {
        // Line height is typically 120% of font size
        lineHeight_ = static_cast<int>(size * 1.2f);
        
        // Initialize default character info for all printable ASCII characters
        initializeDefaultCharacterInfo();
    }
    virtual ~Font() = default;
    
    const std::string& getName() const { return name_; }
    int getSize() const { return size_; }
    int getLineHeight() const { return lineHeight_; }
    unsigned int getTextureId() const { return textureId_; }
    
    int getTextWidth(const std::string& text) const {
        if (text.empty()) return 0;
        
        // Simple placeholder implementation
        // In real implementation, this would use font metrics
        // For now, approximate each character as size/2 wide
        return static_cast<int>(text.length() * size_ / 2);
    }
    
    bool loadFromAtlasData(const std::vector<uint8_t>& atlasData, int width, int height) {
        // Simulate loading atlas data and creating OpenGL texture
        // In real implementation, this would create actual OpenGL texture
        if (atlasData.size() != static_cast<size_t>(width * height)) {
            return false;
        }
        
        // Mock texture ID (in real implementation, would use glGenTextures)
        textureId_ = 1;
        
        return true;
    }
    
    CharacterInfo getCharacterInfo(char c) const {
        // Return character info for printable ASCII characters
        if (c >= 32 && c <= 126) {
            return characterInfo_[c - 32];
        }
        
        // Return default info for non-printable characters
        return CharacterInfo{size_/2, size_, 0.0f, 0.0f, 0.1f, 0.1f, 0, 0, size_/2};
    }
    
private:
    void initializeDefaultCharacterInfo() {
        // Initialize character info for printable ASCII characters (32-126)
        characterInfo_.resize(95);
        
        for (int i = 0; i < 95; ++i) {
            CharacterInfo& info = characterInfo_[i];
            info.width = size_ / 2;
            info.height = size_;
            info.u = (i % 16) * 0.0625f;  // 16 chars per row in a typical font atlas
            info.v = (i / 16) * 0.0625f;  // 6 rows for 95 chars
            info.uWidth = 0.0625f;
            info.vHeight = 0.0625f;
            info.xOffset = 0;
            info.yOffset = 0;
            info.advance = size_ / 2;
        }
    }
    
    std::string name_;
    int size_;
    int lineHeight_;
    unsigned int textureId_;
    std::vector<CharacterInfo> characterInfo_;
};

} // namespace d2