#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "ui/font.h"

namespace d2 {

class FontManager {
public:
    FontManager() = default;
    virtual ~FontManager() = default;
    
    size_t getFontCount() const { return fonts_.size(); }
    
    std::shared_ptr<Font> getFont(const std::string& name) const {
        auto it = fonts_.find(name);
        return (it != fonts_.end()) ? it->second : nullptr;
    }
    
    bool hasFont(const std::string& name) const {
        return fonts_.find(name) != fonts_.end();
    }
    
    std::shared_ptr<Font> createFont(const std::string& name, int size) {
        // If font already exists with this name, return it
        auto existing = getFont(name);
        if (existing) {
            return existing;
        }
        
        // Create new font
        auto font = std::make_shared<Font>(name, size);
        fonts_[name] = font;
        return font;
    }
    
    bool loadDefaultFonts() {
        // Load standard Diablo II font sizes
        // In real implementation, this would load actual font files from assets
        
        // Create default font atlas data (placeholder)
        std::vector<uint8_t> defaultAtlasData(256 * 256, 200);
        
        // Create default fonts with different sizes
        auto defaultFont = createFont("default", 16);
        auto largeFont = createFont("large", 24);
        auto smallFont = createFont("small", 12);
        
        // Load atlas data for each font
        if (!defaultFont->loadFromAtlasData(defaultAtlasData, 256, 256) ||
            !largeFont->loadFromAtlasData(defaultAtlasData, 256, 256) ||
            !smallFont->loadFromAtlasData(defaultAtlasData, 256, 256)) {
            return false;
        }
        
        return true;
    }
    
    bool loadFontFromAtlas(const std::string& name, int size, 
                          const std::vector<uint8_t>& atlasData, 
                          int width, int height) {
        // Create new font with the specified name and size
        auto font = std::make_shared<Font>(name, size);
        
        // Load the atlas data
        if (!font->loadFromAtlasData(atlasData, width, height)) {
            return false;
        }
        
        // Store the font
        fonts_[name] = font;
        return true;
    }
    
    // New methods for real implementation
    bool loadDefaultFontsFromDirectory(const std::string& directory);
    bool loadFontFromFile(const std::string& name, const std::string& fontFile);
    void markFontAsPermanent(const std::string& name);
    void releaseUnusedFonts();
    void enableHotReloading(bool enable);
    void checkAndReloadModifiedFonts();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Font>> fonts_;
    std::unordered_map<std::string, bool> permanentFonts_;
    std::unordered_map<std::string, std::filesystem::file_time_type> fontFileTimestamps_;
    std::unordered_map<std::string, std::string> fontFilePaths_;  // Font name to file path
    std::unordered_map<std::string, unsigned int> textureCache_;  // Path to texture ID
    bool hotReloadingEnabled_ = false;
};

} // namespace d2