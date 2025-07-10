#pragma once

#include <string>
#include <functional>

namespace d2 {

/**
 * ExtractionCoordinator - Master class that orchestrates the entire extraction workflow
 * 
 * This class integrates all extraction capabilities from Phases 31-33 to provide
 * a unified workflow for extracting game assets from any supported source type.
 */
class ExtractionCoordinator {
public:
    ExtractionCoordinator() = default;
    ~ExtractionCoordinator() = default;
    
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

private:
    std::function<void(float, const std::string&)> progressCallback;
};

} // namespace d2