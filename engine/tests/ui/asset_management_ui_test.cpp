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

} // namespace ui
} // namespace d2