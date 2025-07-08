#pragma once

#include <memory>
#include <cstdint>
#include <unordered_map>

namespace d2::sprites {
class DC6Sprite;
}

namespace d2::rendering {

struct TextureInfo {
    uint32_t width;
    uint32_t height;
    uint32_t gl_texture_id;
};

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager() = default;

    uint32_t uploadSprite(std::shared_ptr<sprites::DC6Sprite> sprite, uint32_t direction, uint32_t frame);
    bool isTextureValid(uint32_t texture_id) const;
    
    // Create texture from raw RGBA data
    uint32_t createTexture(const uint8_t* rgba_data, uint32_t width, uint32_t height);
    uint32_t getTextureWidth(uint32_t texture_id) const;
    uint32_t getTextureHeight(uint32_t texture_id) const;

private:
    uint32_t next_texture_id_ = 1;
    std::unordered_map<uint32_t, TextureInfo> textures_;
};

} // namespace d2::rendering