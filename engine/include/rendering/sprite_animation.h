#ifndef D2_RENDERING_SPRITE_ANIMATION_H
#define D2_RENDERING_SPRITE_ANIMATION_H

#include <string>
#include <cstdint>

namespace d2 {
namespace rendering {

/**
 * Manages sprite animation for characters, monsters, and other animated objects
 */
class SpriteAnimation {
public:
    explicit SpriteAnimation(const std::string& spriteName);
    
    // Basic animation state
    const std::string& getSpriteName() const;
    uint32_t getCurrentFrame() const;
    uint32_t getCurrentDirection() const;
    bool isPlaying() const;
    
    // Animation control
    void play();
    void stop();
    
    // Animation configuration
    void setFrameCount(uint32_t frameCount);
    void setFrameRate(float frameRate);
    void setDirection(uint32_t direction);
    
    // Animation update
    void update(float deltaTime);
    
private:
    std::string spriteName_;
    uint32_t currentFrame_;
    uint32_t currentDirection_;
    bool playing_;
    uint32_t frameCount_;
    float frameRate_;
    float timeAccumulator_;
};

} // namespace rendering
} // namespace d2

#endif // D2_RENDERING_SPRITE_ANIMATION_H