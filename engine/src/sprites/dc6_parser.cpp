#include "sprites/dc6_parser.h"
#include <fstream>
#include <filesystem>
#include <cstring>

namespace d2portable {
namespace sprites {

// DC6 RLE decompression
std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& compressed_data, uint32_t expected_size) {
    std::vector<uint8_t> decompressed;
    decompressed.reserve(expected_size);
    
    size_t pos = 0;
    while (pos < compressed_data.size() && decompressed.size() < expected_size) {
        uint8_t command = compressed_data[pos++];
        
        if (pos >= compressed_data.size()) break;
        
        if (command & 0x80) {
            // RLE run: high bit set means repeat next byte
            uint8_t count = command & 0x7F; // Remove high bit to get count
            uint8_t pixel_value = compressed_data[pos++];
            
            // End marker check
            if (count == 0) {
                break;
            }
            
            // Add repeated pixels
            for (uint8_t i = 0; i < count && decompressed.size() < expected_size; i++) {
                decompressed.push_back(pixel_value);
            }
        } else {
            // Raw data: copy next 'command' bytes directly
            for (uint8_t i = 0; i < command && pos < compressed_data.size() && decompressed.size() < expected_size; i++) {
                decompressed.push_back(compressed_data[pos++]);
            }
        }
    }
    
    // Pad with transparent pixels if needed
    while (decompressed.size() < expected_size) {
        decompressed.push_back(0);
    }
    
    return decompressed;
}

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
    
    std::vector<uint8_t> getFrameImageWithPalette(uint32_t direction, uint32_t frame, 
                                                  const std::vector<uint32_t>& palette) const override {
        auto dc6_frame = getFrame(direction, frame);
        std::vector<uint8_t> rgba_data;
        
        if (dc6_frame.pixel_data.empty()) {
            return rgba_data;
        }
        
        // Check if palette is valid (must have 256 colors)
        if (palette.size() != 256) {
            // Fall back to grayscale conversion
            return getFrameImage(direction, frame);
        }
        
        // Convert palette indexed data to RGBA using the provided palette
        rgba_data.reserve(dc6_frame.width * dc6_frame.height * 4);
        
        for (uint8_t pixel_index : dc6_frame.pixel_data) {
            // Get color from palette
            uint32_t color = palette[pixel_index];
            
            // Extract RGBA components
            uint8_t r = color & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 16) & 0xFF;
            uint8_t a = (color >> 24) & 0xFF;
            
            rgba_data.push_back(r);
            rgba_data.push_back(g);
            rgba_data.push_back(b);
            rgba_data.push_back(a);
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
            
            // Read pixel data
            std::vector<uint8_t> raw_data(frame_header.length);
            file.read(reinterpret_cast<char*>(raw_data.data()), 
                      frame_header.length);
            
            // Check if data needs RLE decompression
            uint32_t expected_size = frame_header.width * frame_header.height;
            if (frame_header.length == expected_size) {
                // Data is already uncompressed
                dc6_frame.pixel_data = raw_data;
            } else {
                // Data is RLE compressed
                dc6_frame.pixel_data = decompressRLE(raw_data, expected_size);
            }
            
            sprite->setFrame(dir, frame, dc6_frame);
        }
    }
    
    return sprite;
}

std::unique_ptr<DC6Sprite> DC6Parser::parseData(const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(DC6Header)) {
        return nullptr;
    }
    
    // Read header from buffer
    DC6Header header;
    std::memcpy(&header, data.data(), sizeof(DC6Header));
    
    if (header.version != 6) {
        return nullptr;
    }
    
    // Calculate required size
    size_t required_size = sizeof(DC6Header) + 
                          (header.directions * header.frames_per_dir * sizeof(uint32_t));
    if (data.size() < required_size) {
        return nullptr;
    }
    
    // Read frame pointers
    std::vector<uint32_t> frame_pointers(header.directions * header.frames_per_dir);
    std::memcpy(frame_pointers.data(), 
                data.data() + sizeof(DC6Header), 
                frame_pointers.size() * sizeof(uint32_t));
    
    // Create sprite
    auto sprite = std::make_unique<DC6SpriteImpl>(header.directions, header.frames_per_dir);
    
    // Read each frame
    for (uint32_t dir = 0; dir < header.directions; dir++) {
        for (uint32_t frame = 0; frame < header.frames_per_dir; frame++) {
            uint32_t idx = dir * header.frames_per_dir + frame;
            uint32_t offset = frame_pointers[idx];
            
            if (offset + sizeof(DC6FrameHeader) > data.size()) {
                continue; // Skip invalid frame
            }
            
            // Read frame header
            DC6FrameHeader frame_header;
            std::memcpy(&frame_header, data.data() + offset, sizeof(DC6FrameHeader));
            
            // Create frame
            DC6Frame dc6_frame;
            dc6_frame.width = frame_header.width;
            dc6_frame.height = frame_header.height;
            dc6_frame.offset_x = frame_header.offset_x;
            dc6_frame.offset_y = frame_header.offset_y;
            
            // Check if we have enough data for the pixels
            uint32_t pixel_offset = offset + sizeof(DC6FrameHeader);
            if (pixel_offset + frame_header.length > data.size()) {
                continue; // Skip frame with invalid data
            }
            
            // Extract pixel data
            std::vector<uint8_t> raw_data(data.begin() + pixel_offset,
                                         data.begin() + pixel_offset + frame_header.length);
            
            // Check if data needs RLE decompression
            uint32_t expected_size = frame_header.width * frame_header.height;
            if (frame_header.length == expected_size) {
                // Data is already uncompressed
                dc6_frame.pixel_data = raw_data;
            } else {
                // Data is RLE compressed
                dc6_frame.pixel_data = decompressRLE(raw_data, expected_size);
            }
            
            sprite->setFrame(dir, frame, dc6_frame);
        }
    }
    
    return sprite;
}

std::vector<uint32_t> DC6Parser::getDefaultPalette() const {
    std::vector<uint32_t> palette(256);
    
    // Create a basic Diablo II-style palette
    // Index 0 is always transparent
    palette[0] = 0x00000000; // Transparent (ARGB: alpha=0)
    
    // Create a grayscale palette with some color variation for testing
    for (size_t i = 1; i < 256; i++) {
        uint8_t intensity = static_cast<uint8_t>(i);
        
        // Add some color variation to make it more interesting
        uint8_t r, g, b;
        if (i < 64) {
            // Dark colors with slight red tint
            r = intensity + 20;
            g = intensity;
            b = intensity;
        } else if (i < 128) {
            // Mid-range with slight green tint
            r = intensity;
            g = intensity + 20;
            b = intensity;
        } else if (i < 192) {
            // Brighter with slight blue tint
            r = intensity;
            g = intensity;
            b = intensity + 20;
        } else {
            // Brightest colors - pure intensity
            r = intensity;
            g = intensity;
            b = intensity;
        }
        
        // Ensure we don't overflow
        r = r > 255 ? 255 : r;
        g = g > 255 ? 255 : g;
        b = b > 255 ? 255 : b;
        
        // Store as ARGB (alpha=255 for opaque)
        palette[i] = (255 << 24) | (b << 16) | (g << 8) | r;
    }
    
    return palette;
}

} // namespace sprites
} // namespace d2portable