#pragma once

#include <string>

namespace d2 {
namespace onboarding {

enum class AssetType {
    SPRITES,
    AUDIO,
    DATA_TABLES,
    VIDEOS
};

/**
 * Configuration options for asset extraction
 * Allows users to customize extraction paths and select specific asset types
 */
class ExtractionOptions {
public:
    ExtractionOptions() = default;
    
    /**
     * Get the custom output path for extraction
     * @return Output path, empty string means use default
     */
    std::string getOutputPath() const { return output_path_; }
    
    /**
     * Set a custom output path for extraction
     * @param path The custom path where assets should be extracted
     */
    void setOutputPath(const std::string& path) { output_path_ = path; }
    
    /**
     * Check if a specific asset type is enabled for extraction
     * @param type The asset type to check
     * @return true if the asset type should be extracted
     */
    bool isAssetTypeEnabled(AssetType type) const {
        // By default, all asset types are enabled
        return true;
    }
    
private:
    std::string output_path_ = "";  // Empty means use default
};

} // namespace onboarding
} // namespace d2