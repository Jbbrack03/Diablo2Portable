#ifndef D2PORTABLE_DC6_PARSER_H
#define D2PORTABLE_DC6_PARSER_H

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace d2portable {
namespace sprites {

/**
 * Frame information for a DC6 sprite
 */
struct DC6Frame {
    uint32_t width;
    uint32_t height;
    int32_t offset_x;
    int32_t offset_y;
    std::vector<uint8_t> pixel_data;
};

/**
 * DC6 Sprite class representing a loaded sprite
 */
class DC6Sprite {
public:
    virtual ~DC6Sprite() = default;
    
    /**
     * Get the number of directions in the sprite
     */
    virtual uint32_t getDirectionCount() const = 0;
    
    /**
     * Get the number of frames per direction
     */
    virtual uint32_t getFramesPerDirection() const = 0;
    
    /**
     * Get a specific frame
     * @param direction Direction index
     * @param frame Frame index
     * @return Frame information
     */
    virtual DC6Frame getFrame(uint32_t direction, uint32_t frame) const = 0;
    
    /**
     * Get frame as RGBA image data
     * @param direction Direction index
     * @param frame Frame index
     * @return RGBA pixel data (4 bytes per pixel)
     */
    virtual std::vector<uint8_t> getFrameImage(uint32_t direction, uint32_t frame) const = 0;
};

/**
 * DC6 file parser for Diablo II sprites
 */
class DC6Parser {
public:
    /**
     * Parse a DC6 file from disk
     * @param filepath Path to the DC6 file
     * @return Parsed sprite object, or nullptr on failure
     */
    std::unique_ptr<DC6Sprite> parseFile(const std::string& filepath);
    
    /**
     * Parse DC6 data from memory
     * @param data DC6 file data
     * @return Parsed sprite object, or nullptr on failure
     */
    std::unique_ptr<DC6Sprite> parseData(const std::vector<uint8_t>& data);
};

} // namespace sprites
} // namespace d2portable

#endif // D2PORTABLE_DC6_PARSER_H