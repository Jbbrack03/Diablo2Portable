#include "sprites/dc6_parser.h"
#include <fstream>
#include <filesystem>

namespace d2portable {
namespace sprites {

// DC6 file header structure
struct DC6Header {
    uint32_t version;
    uint32_t flags;
    uint32_t encoding;
    uint32_t termination;
    uint32_t directions;
    uint32_t frames_per_dir;
};

// DC6 frame header structure
struct DC6FrameHeader {
    uint32_t flip;
    uint32_t width;
    uint32_t height;
    int32_t offset_x;
    int32_t offset_y;
    uint32_t allocsize;
    uint32_t next_block;
    uint32_t length;
};

// Implementation of DC6Sprite
class DC6SpriteImpl : public DC6Sprite {
public:
    DC6SpriteImpl(uint32_t dirs, uint32_t frames) 
        : directions(dirs), frames_per_dir(frames) {
        frames_data.resize(dirs);
        for (auto& dir : frames_data) {
            dir.resize(frames_per_dir);
        }
    }
    
    uint32_t getDirectionCount() const override {
        return directions;
    }
    
    uint32_t getFramesPerDirection() const override {
        return frames_per_dir;
    }
    
    DC6Frame getFrame(uint32_t direction, uint32_t frame) const override {
        if (direction >= directions || frame >= frames_per_dir) {
            return DC6Frame{};
        }
        return frames_data[direction][frame];
    }
    
    std::vector<uint8_t> getFrameImage(uint32_t direction, uint32_t frame) const override {
        auto dc6_frame = getFrame(direction, frame);
        std::vector<uint8_t> rgba_data;
        
        if (dc6_frame.pixel_data.empty()) {
            return rgba_data;
        }
        
        // Convert palette indexed data to RGBA
        rgba_data.reserve(dc6_frame.width * dc6_frame.height * 4);
        
        for (uint8_t pixel : dc6_frame.pixel_data) {
            // For now, just use grayscale (no palette)
            rgba_data.push_back(pixel);  // R
            rgba_data.push_back(pixel);  // G
            rgba_data.push_back(pixel);  // B
            rgba_data.push_back(255);     // A
        }
        
        return rgba_data;
    }
    
    void setFrame(uint32_t direction, uint32_t frame, const DC6Frame& frame_data) {
        if (direction < directions && frame < frames_per_dir) {
            frames_data[direction][frame] = frame_data;
        }
    }
    
private:
    uint32_t directions;
    uint32_t frames_per_dir;
    std::vector<std::vector<DC6Frame>> frames_data;
};

// DC6Parser implementation
std::unique_ptr<DC6Sprite> DC6Parser::parseFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return nullptr;
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // Read header
    DC6Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(DC6Header));
    
    if (!file.good() || header.version != 6) {
        return nullptr;
    }
    
    // Read frame pointers
    std::vector<uint32_t> frame_pointers(header.directions * header.frames_per_dir);
    file.read(reinterpret_cast<char*>(frame_pointers.data()), 
              frame_pointers.size() * sizeof(uint32_t));
    
    // Create sprite
    auto sprite = std::make_unique<DC6SpriteImpl>(header.directions, header.frames_per_dir);
    
    // Read each frame
    for (uint32_t dir = 0; dir < header.directions; dir++) {
        for (uint32_t frame = 0; frame < header.frames_per_dir; frame++) {
            uint32_t idx = dir * header.frames_per_dir + frame;
            file.seekg(frame_pointers[idx]);
            
            // Read frame header
            DC6FrameHeader frame_header;
            file.read(reinterpret_cast<char*>(&frame_header), sizeof(DC6FrameHeader));
            
            // Read pixel data
            DC6Frame dc6_frame;
            dc6_frame.width = frame_header.width;
            dc6_frame.height = frame_header.height;
            dc6_frame.offset_x = frame_header.offset_x;
            dc6_frame.offset_y = frame_header.offset_y;
            dc6_frame.pixel_data.resize(frame_header.length);
            
            file.read(reinterpret_cast<char*>(dc6_frame.pixel_data.data()), 
                      frame_header.length);
            
            sprite->setFrame(dir, frame, dc6_frame);
        }
    }
    
    return sprite;
}

std::unique_ptr<DC6Sprite> DC6Parser::parseData(const std::vector<uint8_t>& data) {
    // TODO: Implement parsing from memory buffer
    return nullptr;
}

} // namespace sprites
} // namespace d2portable