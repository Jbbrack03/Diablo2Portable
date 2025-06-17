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

TEST_F(FontManagerTest, CreateAndRetrieveFont) {
    FontManager fontManager;
    
    // Create a font
    auto font = fontManager.createFont("Arial", 16);
    EXPECT_NE(font, nullptr);
    EXPECT_EQ(font->getName(), "Arial");
    EXPECT_EQ(font->getSize(), 16);
    
    // Font count should increase
    EXPECT_EQ(fontManager.getFontCount(), 1);
    
    // Should be able to retrieve the same font
    auto retrievedFont = fontManager.getFont("Arial");
    EXPECT_EQ(retrievedFont, font);
    
    // Creating font with same name should return existing one
    auto duplicateFont = fontManager.createFont("Arial", 24);
    EXPECT_EQ(duplicateFont, font);
    EXPECT_EQ(fontManager.getFontCount(), 1);
}

} // namespace d2