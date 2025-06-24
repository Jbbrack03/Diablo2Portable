#pragma once

#include <string>
#include <vector>
#include <functional>
#include <filesystem>

namespace d2 {

/**
 * AssetExtractor - Extracts game assets from Diablo II MPQ files
 * 
 * This tool extracts sprites, sounds, and data tables from the original
 * Diablo II MPQ archives and organizes them for mobile optimization.
 */
class AssetExtractor {
public:
    AssetExtractor() = default;
    ~AssetExtractor() = default;
    
    /**
     * Extract all game assets from Diablo II installation
     * @param d2Path Path to Diablo II installation directory
     * @param outputPath Path where extracted assets will be saved
     * @return true if extraction succeeded, false otherwise
     */
    bool extractFromD2(const std::string& d2Path, const std::string& outputPath);
    
    /**
     * Get the number of files extracted
     * @return Number of extracted files
     */
    size_t getExtractedFileCount() const { return extractedCount; }
    
    /**
     * Set progress callback for extraction updates
     * @param callback Function called with progress (0.0-1.0) and current file
     */
    void setProgressCallback(std::function<void(float, const std::string&)> callback) {
        progressCallback = callback;
    }
    
private:
    size_t extractedCount = 0;
    std::function<void(float, const std::string&)> progressCallback;
    
    // Helper methods
    bool validateD2Path(const std::filesystem::path& path) const;
    bool createOutputDirectories(const std::filesystem::path& outputPath) const;
    bool extractMPQFiles(const std::filesystem::path& d2Path, 
                        const std::filesystem::path& outputPath);
    bool extractSprites(const std::filesystem::path& mpqPath,
                       const std::filesystem::path& outputPath);
    bool extractSounds(const std::filesystem::path& mpqPath,
                      const std::filesystem::path& outputPath);
    bool extractDataTables(const std::filesystem::path& mpqPath,
                          const std::filesystem::path& outputPath);
    
    void reportProgress(float progress, const std::string& currentFile);
};

} // namespace d2