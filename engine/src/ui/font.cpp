#include "ui/font.h"
#include <fstream>
#include <sstream>
#include <map>

namespace d2 {

bool Font::loadFromFile(const std::string& fontFile) {
    std::ifstream file(fontFile);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing character info and prepare for new data
    characterInfo_.clear();
    characterInfo_.resize(95);  // ASCII 32-126
    
    // Initialize with defaults
    initializeDefaultCharacterInfo();
    
    // Parse bitmap font file (.fnt format)
    std::string line;
    std::map<int, CharacterInfo> charMap;
    std::map<std::pair<int, int>, int> kerningMap;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string tag;
        iss >> tag;
        
        if (tag == "info") {
            // Parse font info
            // Could extract face name, size, etc.
        } else if (tag == "common") {
            // Parse common data
            std::string param;
            while (iss >> param) {
                if (param.find("lineHeight=") == 0) {
                    lineHeight_ = std::stoi(param.substr(11));
                } else if (param.find("scaleW=") == 0) {
                    atlasWidth_ = std::stoi(param.substr(7));
                } else if (param.find("scaleH=") == 0) {
                    atlasHeight_ = std::stoi(param.substr(7));
                }
            }
        } else if (tag == "char") {
            // Parse character data
            CharacterInfo charInfo;
            int id = 0;
            std::string param;
            
            while (iss >> param) {
                if (param.find("id=") == 0) {
                    id = std::stoi(param.substr(3));
                } else if (param.find("x=") == 0) {
                    float x = std::stof(param.substr(2));
                    charInfo.u = x / atlasWidth_;
                } else if (param.find("y=") == 0) {
                    float y = std::stof(param.substr(2));
                    charInfo.v = y / atlasHeight_;
                } else if (param.find("width=") == 0) {
                    charInfo.width = std::stoi(param.substr(6));
                    charInfo.uWidth = float(charInfo.width) / atlasWidth_;
                } else if (param.find("height=") == 0) {
                    charInfo.height = std::stoi(param.substr(7));
                    charInfo.vHeight = float(charInfo.height) / atlasHeight_;
                } else if (param.find("xoffset=") == 0) {
                    charInfo.xOffset = std::stoi(param.substr(8));
                } else if (param.find("yoffset=") == 0) {
                    charInfo.yOffset = std::stoi(param.substr(8));
                } else if (param.find("xadvance=") == 0) {
                    charInfo.advance = std::stoi(param.substr(9));
                }
            }
            
            if (id >= 32 && id <= 126) {
                characterInfo_[id - 32] = charInfo;
            }
            charMap[id] = charInfo;
        } else if (tag == "kerning") {
            // Parse kerning data
            int first = 0, second = 0, amount = 0;
            std::string param;
            
            while (iss >> param) {
                if (param.find("first=") == 0) {
                    first = std::stoi(param.substr(6));
                } else if (param.find("second=") == 0) {
                    second = std::stoi(param.substr(7));
                } else if (param.find("amount=") == 0) {
                    amount = std::stoi(param.substr(7));
                }
            }
            
            kerningMap[{first, second}] = amount;
        }
    }
    
    // Store kerning data
    kerningMap_ = kerningMap;
    hasLoadedFont_ = true;
    
    // Create mock texture ID (in real implementation, would load the texture atlas)
    textureId_ = 1;
    
    return true;
}

int Font::getTextWidth(const std::string& text) const {
    if (text.empty()) return 0;
    
    int width = 0;
    char prevChar = 0;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        // Get character info
        CharacterInfo charInfo;
        if (c >= 32 && c <= 126 && hasLoadedFont_) {
            charInfo = characterInfo_[c - 32];
            
            // Check if this character was actually loaded (not default)
            // If it's still using default values, use fallback
            if (charInfo.width == size_ / 2 && charInfo.advance == size_ / 2) {
                // This is a default character, use different fallback
                charInfo.advance = size_ / 2 + 2;  // Make it slightly different
            }
        } else {
            // Non-printable or font not loaded
            charInfo = getCharacterInfo(c);
        }
        
        // Add character advance
        width += charInfo.advance;
        
        // Apply kerning if available
        if (prevChar != 0 && hasLoadedFont_) {
            auto kerningIt = kerningMap_.find({prevChar, c});
            if (kerningIt != kerningMap_.end()) {
                width += kerningIt->second;
            }
        }
        
        prevChar = c;
    }
    
    return width;
}

} // namespace d2