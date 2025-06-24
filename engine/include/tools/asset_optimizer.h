#pragma once

#include <string>
#include <memory>

namespace d2 {

/**
 * AssetOptimizer - Optimizes game assets for mobile devices
 * 
 * This tool compresses and optimizes sprites, sounds, and other assets
 * to reduce APK size and improve loading performance on mobile devices.
 */
class AssetOptimizer {
public:
    enum class CompressionType {
        PNG,     // For sprites with transparency
        JPEG,    // For opaque backgrounds  
        WEBP,    // Modern format with good compression
        PVR      // PowerVR texture compression for mobile
    };
    
    AssetOptimizer();
    ~AssetOptimizer();
    
    /**
     * Optimize a sprite file for mobile
     * @param inputPath Path to the input sprite (DC6 format)
     * @param outputPath Path where optimized sprite will be saved
     * @return true if optimization succeeded, false otherwise
     */
    bool optimizeSprite(const std::string& inputPath, const std::string& outputPath);
    
    /**
     * Get the compression ratio achieved
     * @return Compression ratio (0.0 - 1.0, where 1.0 = 100% space saved)
     */
    float getCompressionRatio() const { return compressionRatio; }
    
    /**
     * Set the compression type to use
     * @param type Compression type to use for optimization
     */
    void setCompressionType(CompressionType type) { compressionType = type; }
    
    /**
     * Set quality level for lossy compression
     * @param quality Quality level (0-100, where 100 = best quality)
     */
    void setQuality(int quality) { this->quality = quality; }
    
private:
    float compressionRatio = 0.0f;
    CompressionType compressionType = CompressionType::PNG;
    int quality = 85; // Default quality for lossy compression
    
    // Helper methods
    bool loadDC6Sprite(const std::string& inputPath);
    bool saveOptimizedSprite(const std::string& outputPath);
    bool compressToPNG(const std::string& outputPath);
    bool compressToPVR(const std::string& outputPath);
    
    // Internal sprite data
    struct SpriteData;
    std::unique_ptr<SpriteData> spriteData;
};

} // namespace d2