#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace d2 {

/**
 * TextureAtlas - Represents a generated texture atlas
 */
class TextureAtlas {
public:
    struct SpriteInfo {
        int page;           // Which texture page this sprite is on
        int x, y;          // Position within the page
        int width, height; // Dimensions of the sprite
    };
    
    TextureAtlas() = default;
    ~TextureAtlas() = default;
    
    /**
     * Check if the atlas is valid
     * @return true if atlas was generated successfully
     */
    bool isValid() const { return valid; }
    
    /**
     * Get the number of texture pages in the atlas
     * @return Number of pages
     */
    int getPageCount() const { return pageCount; }
    
    /**
     * Check if a sprite exists in the atlas
     * @param spriteName Name of the sprite to check
     * @return true if sprite exists in atlas
     */
    bool hasSprite(const std::string& spriteName) const {
        return spriteMap.find(spriteName) != spriteMap.end();
    }
    
    /**
     * Get information about a sprite in the atlas
     * @param spriteName Name of the sprite
     * @return Sprite information (position, size, page)
     */
    const SpriteInfo* getSpriteInfo(const std::string& spriteName) const {
        auto it = spriteMap.find(spriteName);
        return (it != spriteMap.end()) ? &it->second : nullptr;
    }
    
private:
    friend class TextureAtlasGenerator;
    
    bool valid = false;
    int pageCount = 0;
    std::unordered_map<std::string, SpriteInfo> spriteMap;
};

/**
 * TextureAtlasGenerator - Generates texture atlases from sprite files
 * 
 * This tool packs multiple sprites into texture atlas pages to reduce
 * draw calls and improve rendering performance on mobile devices.
 */
class TextureAtlasGenerator {
public:
    TextureAtlasGenerator();
    ~TextureAtlasGenerator();
    
    /**
     * Generate a texture atlas from a list of sprite files
     * @param sprites List of sprite file paths
     * @param maxWidth Maximum width of each atlas page
     * @param maxHeight Maximum height of each atlas page
     * @return Generated texture atlas
     */
    TextureAtlas generateAtlas(const std::vector<std::string>& sprites, 
                              int maxWidth, int maxHeight);
    
    /**
     * Save the generated atlas to disk
     * @param atlas The atlas to save
     * @param outputPath Base path for output files (pages will be numbered)
     * @return true if save succeeded
     */
    bool saveAtlas(const TextureAtlas& atlas, const std::string& outputPath);
    
    /**
     * Set padding between sprites in the atlas
     * @param padding Padding in pixels
     */
    void setPadding(int padding) { this->padding = padding; }
    
    /**
     * Enable/disable power-of-two texture dimensions
     * @param enable true to force power-of-two dimensions
     */
    void setPowerOfTwo(bool enable) { powerOfTwo = enable; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    int padding = 2;        // Default 2 pixel padding between sprites
    bool powerOfTwo = true; // Default to power-of-two textures for compatibility
};

} // namespace d2