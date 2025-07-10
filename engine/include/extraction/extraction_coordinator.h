#pragma once

#include <string>

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
};

} // namespace d2