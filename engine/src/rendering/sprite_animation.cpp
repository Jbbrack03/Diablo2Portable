#include "rendering/sprite_animation.h"

namespace d2 {
namespace rendering {

SpriteAnimation::SpriteAnimation(const std::string& spriteName) 
    : spriteName_(spriteName), currentFrame_(0), currentDirection_(0), playing_(false),
      frameCount_(1), frameRate_(1.0f), timeAccumulator_(0.0f) {
}

const std::string& SpriteAnimation::getSpriteName() const {
    return spriteName_;
}

uint32_t SpriteAnimation::getCurrentFrame() const {
    return currentFrame_;
}

uint32_t SpriteAnimation::getCurrentDirection() const {
    return currentDirection_;
}

bool SpriteAnimation::isPlaying() const {
    return playing_;
}

float SpriteAnimation::getFrameInterpolation() const {
    if (!playing_ || frameCount_ <= 1 || frameRate_ <= 0.0f) {
        return 0.0f;
    }
    
    float frameDuration = 1.0f / frameRate_;
    return timeAccumulator_ / frameDuration;
}

void SpriteAnimation::play() {
    playing_ = true;
}

void SpriteAnimation::stop() {
    playing_ = false;
}

void SpriteAnimation::setFrameCount(uint32_t frameCount) {
    frameCount_ = frameCount;
    // Reset to frame 0 if current frame is beyond new count
    if (currentFrame_ >= frameCount_) {
        currentFrame_ = 0;
    }
}

void SpriteAnimation::setFrameRate(float frameRate) {
    frameRate_ = frameRate;
}

void SpriteAnimation::setDirection(uint32_t direction) {
    currentDirection_ = direction;
}

void SpriteAnimation::setOnAnimationComplete(std::function<void(const std::string&)> callback) {
    onAnimationComplete_ = callback;
}

void SpriteAnimation::update(float deltaTime) {
    if (!playing_ || frameCount_ <= 1 || frameRate_ <= 0.0f) {
        return;
    }
    
    timeAccumulator_ += deltaTime;
    float frameDuration = 1.0f / frameRate_;
    
    while (timeAccumulator_ >= frameDuration) {
        timeAccumulator_ -= frameDuration;
        uint32_t previousFrame = currentFrame_;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
        
        // Check if we completed a full cycle (wrapped from last frame to frame 0)
        if (previousFrame == frameCount_ - 1 && currentFrame_ == 0) {
            if (onAnimationComplete_) {
                onAnimationComplete_(spriteName_);
            }
        }
    }
}

} // namespace rendering
} // namespace d2