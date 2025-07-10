#pragma once

#include <string>
#include <functional>
#include <memory>

namespace d2 {

// Forward declarations for existing extraction systems
class ISOExtractor;
class PatchSystem;
class AssetExtractor;

/**
 * ExtractionCoordinator - Master class that orchestrates the entire extraction workflow
 * 
 * This class integrates all extraction capabilities from Phases 31-33 to provide
 * a unified workflow for extracting game assets from any supported source type.
 */
class ExtractionCoordinator {
public:
    ExtractionCoordinator();
    ~ExtractionCoordinator();
    
    /**
     * Extract game assets from any supported source to output directory
     * @param sourcePath Path to source file (ISO, MPQ, directory, etc.)
     * @param outputPath Path where extracted assets will be saved
     * @return true if extraction succeeded, false otherwise
     */
    bool extractFrom(const std::string& sourcePath, const std::string& outputPath);
    
    /**
     * Detect the type of source based on file extension or content
     * @param sourcePath Path to source file
     * @return String indicating source type ("ISO", "MPQ", "DIRECTORY", etc.)
     */
    std::string detectSourceType(const std::string& sourcePath);
    
    /**
     * Set progress callback for extraction updates
     * @param callback Function called with progress (0.0-1.0) and current file
     */
    void setProgressCallback(std::function<void(float, const std::string&)> callback);
    
    /**
     * Check if ISO extractor is available
     * @return true if ISO extraction is supported
     */
    bool hasISOExtractor() const;
    
    /**
     * Check if patch system is available
     * @return true if patch processing is supported
     */
    bool hasPatchSystem() const;
    
    /**
     * Check if asset extractor is available
     * @return true if asset extraction is supported
     */
    bool hasAssetExtractor() const;

private:
    std::function<void(float, const std::string&)> progressCallback;
    
    // Instances of existing extraction systems
    std::unique_ptr<ISOExtractor> isoExtractor;
    std::unique_ptr<PatchSystem> patchSystem;
    std::unique_ptr<AssetExtractor> assetExtractor;
};

} // namespace d2