#pragma once

#include <memory>
#include <cstdint>
#include <unordered_map>
#include "sprites/dc6_sprite_wrapper.h"

namespace d2::rendering {

enum class TextureWrapMode {
    CLAMP_TO_EDGE,
    REPEAT
};

struct TextureInfo {
    uint32_t width;
    uint32_t height;
    uint32_t gl_texture_id;
};

class Renderer;

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager() = default;

    bool initialize(const Renderer& renderer);
    uint32_t uploadSprite(std::shared_ptr<sprites::DC6Sprite> sprite, uint32_t direction, uint32_t frame);
    uint32_t uploadSpriteWithPalette(std::shared_ptr<sprites::DC6Sprite> sprite, 
                                     uint32_t direction, uint32_t frame,
                                     const std::vector<uint32_t>& palette);
    bool isTextureValid(uint32_t texture_id) const;
    
    // Create texture from raw RGBA data
    uint32_t createTexture(const uint8_t* rgba_data, uint32_t width, uint32_t height);
    uint32_t getTextureWidth(uint32_t texture_id) const;
    uint32_t getTextureHeight(uint32_t texture_id) const;
    
    // Texture wrapping modes
    void setTextureWrapMode(uint32_t texture_id, TextureWrapMode wrap_mode);

private:
    uint32_t next_texture_id_ = 1;
    std::unordered_map<uint32_t, TextureInfo> textures_;
};

} // namespace d2::rendering