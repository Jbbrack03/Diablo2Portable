#pragma once

#include "sprites/dc6_parser.h"

namespace d2::rendering {
namespace test {

// Mock DC6Sprite for testing
class MockDC6Sprite : public d2portable::sprites::DC6Sprite {
public:
    MockDC6Sprite(uint32_t directions = 1, uint32_t frames_per_dir = 1, 
                  uint32_t width = 2, uint32_t height = 2) 
        : directions_(directions), frames_per_dir_(frames_per_dir),
          width_(width), height_(height) {
        // Create simple test data
        pixel_data_.resize(width * height * 4);
        for (size_t i = 0; i < pixel_data_.size(); i += 4) {
            pixel_data_[i] = 255;     // R
            pixel_data_[i+1] = 0;     // G  
            pixel_data_[i+2] = 0;     // B
            pixel_data_[i+3] = 255;   // A
        }
    }
    
    uint32_t getDirectionCount() const override { return directions_; }
    uint32_t getFramesPerDirection() const override { return frames_per_dir_; }
    
    d2portable::sprites::DC6Frame getFrame(uint32_t direction, uint32_t frame) const override {
        d2portable::sprites::DC6Frame frame_info;
        frame_info.width = width_;
        frame_info.height = height_;
        frame_info.offsetX = 0;
        frame_info.offsetY = 0;
        frame_info.pixelData = pixel_data_;
        return frame_info;
    }
    
    std::vector<uint8_t> getFrameImage(uint32_t direction, uint32_t frame) const override {
        return pixel_data_;
    }
    
    std::vector<uint8_t> getFrameImageWithPalette(uint32_t direction, uint32_t frame, 
                                                  const std::vector<uint32_t>& palette) const override {
        return pixel_data_; // For simplicity, return the same data
    }
    
private:
    uint32_t directions_;
    uint32_t frames_per_dir_;
    uint32_t width_;
    uint32_t height_;
    std::vector<uint8_t> pixel_data_;
};

} // namespace test
} // namespace d2::rendering