#pragma once

#include <string>
#include <memory>
#include <vector>

namespace d2 {

/**
 * Result of a format conversion operation
 */
struct ConversionResult {
    bool success = false;
    std::string format;
    float compressionRatio = 0.0f; // Size reduction ratio (0-1)
    std::string errorMessage;
};

/**
 * Color structure for palette entries
 */
struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t alpha = 255;
};

/**
 * Palette data extracted from D2 palette files
 */
struct Palette {
    int colorCount = 0;
    bool hasTransparency = false;
    std::vector<Color> colors;
};

/**
 * Audio format types
 */
enum class AudioFormat {
    UNKNOWN,
    PCM_8,
    PCM_16,
    PCM_24,
    PCM_32,
    FLOAT_32
};

/**
 * Audio data extracted from sound files
 */
struct AudioData {
    bool isValid = false;
    AudioFormat format = AudioFormat::UNKNOWN;
    int channels = 0;
    int sampleRate = 0;
    std::vector<uint8_t> samples;
};

/**
 * MultiFormatProcessor - Converts DC6 sprites to mobile-optimized formats
 * 
 * Features:
 * - DC6 to PNG conversion with transparency
 * - DC6 to PVR (PowerVR) for mobile GPUs
 * - Batch processing with progress tracking
 * - Quality settings for lossy compression
 * - Format auto-selection based on content
 */
class MultiFormatProcessor {
public:
    MultiFormatProcessor() = default;
    ~MultiFormatProcessor() = default;
    
    /**
     * Convert DC6 sprite to PNG format
     * @param dc6Path Input DC6 file path
     * @param pngPath Output PNG file path
     * @return Conversion result
     */
    ConversionResult convertDC6ToPNG(const std::string& dc6Path, 
                                     const std::string& pngPath);
    
    /**
     * Extract palette from a D2 palette file
     * @param palettePath Path to the palette file
     * @return Extracted palette data
     */
    Palette extractPalette(const std::string& palettePath);
    
    /**
     * Extract audio data from a sound file
     * @param audioPath Path to the audio file
     * @return Extracted audio data
     */
    AudioData extractAudio(const std::string& audioPath);
    
    /**
     * Convert DC6 sprite to PVR format for mobile GPUs
     * @param dc6Path Input DC6 file path
     * @param pvrPath Output PVR file path
     * @return Conversion result
     */
    ConversionResult convertDC6ToPVR(const std::string& dc6Path, 
                                     const std::string& pvrPath);
};

} // namespace d2