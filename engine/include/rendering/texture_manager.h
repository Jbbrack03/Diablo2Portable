#pragma once

#include <memory>
#include <cstdint>

namespace d2::sprites {
class DC6Sprite;
}

namespace d2::rendering {

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager() = default;

    uint32_t uploadSprite(std::shared_ptr<sprites::DC6Sprite> sprite, uint32_t direction, uint32_t frame);
    bool isTextureValid(uint32_t texture_id) const;

private:
    uint32_t next_texture_id_ = 1;
};

} // namespace d2::rendering