#include <gtest/gtest.h>
#include "ui/asset_management_ui.h"

namespace d2 {
namespace ui {

// Test 1: Create AssetManagementUI instance
TEST(AssetManagementUITest, CreateAssetManagementUI) {
    AssetManagementUI ui;
    
    // Should be initialized and ready
    EXPECT_TRUE(ui.isInitialized());
}

// Test 2: Track extraction progress
TEST(AssetManagementUITest, TrackExtractionProgress) {
    AssetManagementUI ui;
    
    // Set extraction progress
    ui.setExtractionProgress("d2data.mpq", 0.5f, 50, 100);
    
    auto progress = ui.getExtractionProgress("d2data.mpq");
    EXPECT_FLOAT_EQ(progress.percentage, 0.5f);
    EXPECT_EQ(progress.filesExtracted, 50);
    EXPECT_EQ(progress.totalFiles, 100);
    EXPECT_EQ(progress.currentFile, "d2data.mpq");
}

// Test 3: Asset validation interface
TEST(AssetManagementUITest, AssetValidationInterface) {
    AssetManagementUI ui;
    
    // Start validation
    ui.startAssetValidation("/path/to/assets");
    
    // Check validation status
    auto status = ui.getValidationStatus();
    EXPECT_TRUE(status.isValidating);
    EXPECT_EQ(status.assetPath, "/path/to/assets");
    
    // Set validation results
    ui.setValidationResult(1000, 5, {"missing_file1.dc6", "missing_file2.dc6"});
    
    status = ui.getValidationStatus();
    EXPECT_FALSE(status.isValidating);
    EXPECT_EQ(status.totalAssets, 1000);
    EXPECT_EQ(status.missingAssets, 5);
    EXPECT_EQ(status.missingFiles.size(), 2);
    EXPECT_EQ(status.missingFiles[0], "missing_file1.dc6");
}

// Test 4: Storage management tools
TEST(AssetManagementUITest, StorageManagementTools) {
    AssetManagementUI ui;
    
    // Set storage information
    ui.updateStorageInfo(1024 * 1024 * 1024, 512 * 1024 * 1024); // 1GB total, 512MB used
    
    auto storage = ui.getStorageInfo();
    EXPECT_EQ(storage.totalSpace, 1024 * 1024 * 1024);
    EXPECT_EQ(storage.usedSpace, 512 * 1024 * 1024);
    EXPECT_EQ(storage.availableSpace, 512 * 1024 * 1024);
    EXPECT_FLOAT_EQ(storage.usagePercentage, 50.0f);
    
    // Check if enough space for extraction
    EXPECT_TRUE(ui.hasEnoughSpaceForExtraction(256 * 1024 * 1024)); // 256MB needed
    EXPECT_FALSE(ui.hasEnoughSpaceForExtraction(600 * 1024 * 1024)); // 600MB needed
}

} // namespace ui
} // namespace d2