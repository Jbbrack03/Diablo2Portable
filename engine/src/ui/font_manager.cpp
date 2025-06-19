#include "ui/font_manager.h"
#include <fstream>
#include <algorithm>

namespace d2 {

bool FontManager::loadDefaultFontsFromDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        return false;
    }
    
    bool success = true;
    
    // Look for font files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".fnt") {
            std::string fontName = entry.path().stem().string();
            
            // Load the font file
            if (!loadFontFromFile(fontName, entry.path().string())) {
                success = false;
            }
        }
    }
    
    return success && getFontCount() > 0;
}

bool FontManager::loadFontFromFile(const std::string& name, const std::string& fontFile) {
    // Check if texture is already cached
    std::string texturePath = fontFile.substr(0, fontFile.length() - 4) + ".png";
    unsigned int textureId = 0;
    
    auto texIt = textureCache_.find(texturePath);
    if (texIt != textureCache_.end()) {
        textureId = texIt->second;
    } else {
        // In real implementation, would load texture here
        // For now, use mock texture ID
        textureId = static_cast<unsigned int>(textureCache_.size() + 1);
        textureCache_[texturePath] = textureId;
    }
    
    // First, read the font file to get the size
    std::ifstream file(fontFile);
    if (!file.is_open()) {
        return false;
    }
    
    int fontSize = 16;  // Default
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("info") == 0) {
            // Parse size from info line
            size_t sizePos = line.find("size=");
            if (sizePos != std::string::npos) {
                sizePos += 5;
                size_t endPos = line.find(' ', sizePos);
                if (endPos != std::string::npos) {
                    fontSize = std::stoi(line.substr(sizePos, endPos - sizePos));
                } else {
                    fontSize = std::stoi(line.substr(sizePos));
                }
            }
            break;
        }
    }
    file.close();
    
    // Create font with correct size and load from file
    auto font = std::make_shared<Font>(name, fontSize);
    if (!font->loadFromFile(fontFile)) {
        return false;
    }
    
    // Store the font
    fonts_[name] = font;
    
    // Store file path and timestamp for hot reloading
    fontFilePaths_[name] = fontFile;
    if (hotReloadingEnabled_) {
        fontFileTimestamps_[name] = std::filesystem::last_write_time(fontFile);
    }
    
    return true;
}

void FontManager::markFontAsPermanent(const std::string& name) {
    permanentFonts_[name] = true;
}

void FontManager::releaseUnusedFonts() {
    // Create a list of fonts to remove
    std::vector<std::string> toRemove;
    
    for (const auto& [name, font] : fonts_) {
        // Keep permanent fonts
        if (permanentFonts_.find(name) != permanentFonts_.end()) {
            continue;
        }
        
        // In a real implementation, we might check reference counts
        // For now, remove all non-permanent fonts
        toRemove.push_back(name);
    }
    
    // Remove the fonts
    for (const auto& name : toRemove) {
        fonts_.erase(name);
        fontFileTimestamps_.erase(name);
    }
}

void FontManager::enableHotReloading(bool enable) {
    hotReloadingEnabled_ = enable;
    
    if (enable) {
        // Store current timestamps for all loaded fonts
        for (const auto& [name, font] : fonts_) {
            // This is a simplified version - in reality we'd need to track
            // which file each font came from
        }
    }
}

void FontManager::checkAndReloadModifiedFonts() {
    if (!hotReloadingEnabled_) {
        return;
    }
    
    // Check each font's file for modifications
    for (auto& [name, timestamp] : fontFileTimestamps_) {
        // Get the actual file path for this font
        auto pathIt = fontFilePaths_.find(name);
        if (pathIt == fontFilePaths_.end()) {
            continue;
        }
        
        const std::string& fontFile = pathIt->second;
        
        if (std::filesystem::exists(fontFile)) {
            auto currentTime = std::filesystem::last_write_time(fontFile);
            
            if (currentTime > timestamp) {
                // File has been modified, reload it
                loadFontFromFile(name, fontFile);
                timestamp = currentTime;
            }
        }
    }
}

} // namespace d2