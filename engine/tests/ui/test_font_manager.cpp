#include <gtest/gtest.h>
#include "ui/font_manager.h"

namespace d2 {

class FontManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(FontManagerTest, CreateFontManager) {
    FontManager fontManager;
    
    // Initially should have no fonts loaded
    EXPECT_EQ(fontManager.getFontCount(), 0);
    
    // Getting a non-existent font should return nullptr
    EXPECT_EQ(fontManager.getFont("Arial"), nullptr);
}

} // namespace d2