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

} // namespace ui
} // namespace d2