#pragma once

#include <cstdint>
#include <unordered_set>
#include <glm/vec2.hpp>

namespace d2::rendering {

class Renderer;
class TextureManager;

class SpriteRenderer {
public:
    SpriteRenderer() = default;
    ~SpriteRenderer() = default;

    bool initialize(const Renderer& renderer, const TextureManager& texture_manager);
    virtual void beginFrame();
    virtual void drawSprite(uint32_t texture_id, const glm::vec2& position, const glm::vec2& size);
    virtual void endFrame();
    
    uint32_t getDrawCallCount() const;
    uint32_t getSpriteCount() const;

private:
    bool initialized_ = false;
    uint32_t draw_call_count_ = 0;
    uint32_t sprite_count_ = 0;
    std::unordered_set<uint32_t> textures_used_;
};

} // namespace d2::rendering