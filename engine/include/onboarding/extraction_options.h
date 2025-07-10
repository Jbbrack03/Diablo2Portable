#pragma once

#include <string>
#include <unordered_map>

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
    ExtractionOptions() {
        // Initialize all asset types as enabled by default
        asset_types_[AssetType::SPRITES] = true;
        asset_types_[AssetType::AUDIO] = true;
        asset_types_[AssetType::DATA_TABLES] = true;
        asset_types_[AssetType::VIDEOS] = true;
    }
    
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
     * Enable or disable a specific asset type for extraction
     * @param type The asset type to configure
     * @param enabled Whether the asset type should be extracted
     */
    void setAssetTypeEnabled(AssetType type, bool enabled) {
        asset_types_[type] = enabled;
    }
    
    /**
     * Check if a specific asset type is enabled for extraction
     * @param type The asset type to check
     * @return true if the asset type should be extracted
     */
    bool isAssetTypeEnabled(AssetType type) const {
        auto it = asset_types_.find(type);
        return it != asset_types_.end() ? it->second : true;
    }
    
private:
    std::string output_path_ = "";  // Empty means use default
    mutable std::unordered_map<AssetType, bool> asset_types_;
};

} // namespace onboarding
} // namespace d2