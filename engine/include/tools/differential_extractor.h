#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace d2 {

// Forward declaration
class AssetManifest;

/**
 * Types of changes that can occur to files
 */
enum class ChangeType {
    ADDED,
    MODIFIED,
    DELETED,
    UNCHANGED
};

/**
 * Information about file changes between versions
 */
struct FileChanges {
    std::vector<std::string> modifiedFiles;
    std::vector<std::string> addedFiles;
    std::vector<std::string> deletedFiles;
    
    bool hasFile(const std::string& filename) const {
        for (const auto& file : modifiedFiles) {
            if (file == filename) return true;
        }
        for (const auto& file : addedFiles) {
            if (file == filename) return true;
        }
        for (const auto& file : deletedFiles) {
            if (file == filename) return true;
        }
        return false;
    }
    
    ChangeType changeType(const std::string& filename) const {
        for (const auto& file : modifiedFiles) {
            if (file == filename) return ChangeType::MODIFIED;
        }
        for (const auto& file : addedFiles) {
            if (file == filename) return ChangeType::ADDED;
        }
        for (const auto& file : deletedFiles) {
            if (file == filename) return ChangeType::DELETED;
        }
        return ChangeType::UNCHANGED;
    }
};

/**
 * DifferentialExtractor - Performs incremental asset extraction
 * 
 * Features:
 * - File change detection using checksums and timestamps
 * - Incremental extraction for modified MPQs only
 * - Version compatibility checking
 * - Rollback capability for failed updates
 * - Bandwidth-efficient updates
 */
class DifferentialExtractor {
public:
    DifferentialExtractor() = default;
    ~DifferentialExtractor() = default;
    
    /**
     * Perform full extraction of all assets
     * @param d2Path Path to Diablo II installation
     * @param outputPath Path for extracted assets
     * @return true if extraction succeeded
     */
    bool fullExtraction(const std::string& d2Path, const std::string& outputPath);
    
    /**
     * Generate manifest for extracted assets
     * @param extractedPath Path to extracted assets
     * @return Asset manifest
     */
    std::unique_ptr<AssetManifest> generateManifest(const std::string& extractedPath);
    
    /**
     * Detect changes between current files and manifest
     * @param d2Path Path to Diablo II installation
     * @param baseManifest Previous extraction manifest
     * @return File changes information
     */
    FileChanges detectChanges(const std::string& d2Path, 
                             const AssetManifest& baseManifest);
    
    /**
     * Detect changes in extracted assets compared to manifest
     * @param extractedPath Path to extracted assets
     * @param baseManifest Previous extraction manifest
     * @return File changes information
     */
    FileChanges detectChangesInExtractedAssets(const std::string& extractedPath,
                                              const AssetManifest& baseManifest);

private:
    // Helper methods will be added as needed
};

} // namespace d2