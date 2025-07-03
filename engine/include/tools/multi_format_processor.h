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
};

} // namespace d2