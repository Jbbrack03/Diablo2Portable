#include <gtest/gtest.h>
#include "onboarding/extraction_options.h"

namespace d2 {
namespace onboarding {

class ExtractionOptionsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

// Test 1: Create ExtractionOptions with default settings
TEST_F(ExtractionOptionsTest, CreateWithDefaultSettings) {
    ExtractionOptions options;
    
    // Default output path should be empty (use default)
    EXPECT_EQ("", options.getOutputPath());
    
    // By default, all asset types should be selected
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::SPRITES));
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::AUDIO));
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::DATA_TABLES));
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::VIDEOS));
}

// Test 2: Set custom output path
TEST_F(ExtractionOptionsTest, SetCustomOutputPath) {
    ExtractionOptions options;
    
    // Set a custom output path
    options.setOutputPath("/custom/extraction/path");
    
    EXPECT_EQ("/custom/extraction/path", options.getOutputPath());
}

// Test 3: Disable specific asset types
TEST_F(ExtractionOptionsTest, DisableSpecificAssetTypes) {
    ExtractionOptions options;
    
    // Disable videos and audio
    options.setAssetTypeEnabled(AssetType::VIDEOS, false);
    options.setAssetTypeEnabled(AssetType::AUDIO, false);
    
    // Videos and audio should be disabled
    EXPECT_FALSE(options.isAssetTypeEnabled(AssetType::VIDEOS));
    EXPECT_FALSE(options.isAssetTypeEnabled(AssetType::AUDIO));
    
    // But sprites and data tables should still be enabled
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::SPRITES));
    EXPECT_TRUE(options.isAssetTypeEnabled(AssetType::DATA_TABLES));
}

} // namespace onboarding
} // namespace d2