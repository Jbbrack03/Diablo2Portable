#pragma once

#include <memory>

namespace d2portable::sprites {
    class DC6Sprite;
    struct DC6Frame;
}

namespace d2::animation {

/**
 * Animation directions matching Diablo II's 8-directional system
 */
enum class Direction {
    SOUTH = 0,      // Down
    SOUTH_WEST = 1, // Down-left
    WEST = 2,       // Left
    NORTH_WEST = 3, // Up-left
    NORTH = 4,      // Up
    NORTH_EAST = 5, // Up-right
    EAST = 6,       // Right
    SOUTH_EAST = 7  // Down-right
};

/**
 * Controls sprite animation playback
 * 
 * Manages frame timing, direction changes, and looping for
 * animated DC6 sprites in Diablo II.
 */
class AnimationController {
public:
    /**
     * Create an animation controller for a sprite
     * @param sprite The DC6 sprite to animate
     */
    explicit AnimationController(std::shared_ptr<d2portable::sprites::DC6Sprite> sprite);
    ~AnimationController() = default;
    
    /**
     * Update the animation
     * @param deltaTime Time elapsed since last update in seconds
     */
    void update(float deltaTime);
    
    /**
     * Start playing the animation
     */
    void play();
    
    /**
     * Pause the animation
     */
    void pause();
    
    /**
     * Stop the animation and reset to frame 0
     */
    void stop();
    
    /**
     * Check if animation is currently playing
     */
    bool isPlaying() const { return isPlaying_; }
    
    /**
     * Set the animation direction
     * @param direction The direction to face
     */
    void setDirection(Direction direction);
    
    /**
     * Get the current animation direction
     */
    Direction getCurrentDirection() const { return currentDirection_; }
    
    /**
     * Set the current frame directly
     * @param frame The frame index (0 to frames_per_direction-1)
     */
    void setCurrentFrame(int frame);
    
    /**
     * Get the current frame index
     */
    int getCurrentFrame() const { return currentFrame_; }
    
    /**
     * Set the frame rate (frames per second)
     * @param fps Frames per second
     */
    void setFrameRate(float fps);
    
    /**
     * Get the current frame rate
     */
    float getFrameRate() const { return frameRate_; }
    
    /**
     * Enable or disable looping
     * @param loop True to loop, false to play once
     */
    void setLooping(bool loop) { looping_ = loop; }
    
    /**
     * Check if animation is set to loop
     */
    bool isLooping() const { return looping_; }
    
    /**
     * Get the current sprite frame for rendering
     * @return Pointer to the current DC6Frame
     */
    const d2portable::sprites::DC6Frame* getCurrentSpriteFrame() const;
    
    /**
     * Get the total number of frames in the current direction
     */
    int getTotalFrames() const;
    
    /**
     * Get the sprite being animated
     */
    std::shared_ptr<d2portable::sprites::DC6Sprite> getSprite() const { return sprite_; }
    
private:
    std::shared_ptr<d2portable::sprites::DC6Sprite> sprite_;
    
    Direction currentDirection_ = Direction::SOUTH;
    int currentFrame_ = 0;
    float frameRate_ = 25.0f; // Default D2 animation speed
    float frameTime_ = 0.0f;
    bool isPlaying_ = false;
    bool looping_ = true;
};

} // namespace d2::animation