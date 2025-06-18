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

TEST_F(FontManagerTest, LoadDefaultFontsFromAssets) {
    FontManager fontManager;
    
    // Should be able to load default Diablo II fonts
    EXPECT_TRUE(fontManager.loadDefaultFonts());
    
    // Should have loaded standard font sizes
    EXPECT_TRUE(fontManager.hasFont("default"));
    EXPECT_TRUE(fontManager.hasFont("large"));
    EXPECT_TRUE(fontManager.hasFont("small"));
    
    // Font count should reflect loaded fonts
    EXPECT_GE(fontManager.getFontCount(), 3);
    
    // Retrieved fonts should have proper properties
    auto defaultFont = fontManager.getFont("default");
    EXPECT_NE(defaultFont, nullptr);
    EXPECT_EQ(defaultFont->getSize(), 16);
    
    auto largeFont = fontManager.getFont("large");
    EXPECT_NE(largeFont, nullptr);
    EXPECT_EQ(largeFont->getSize(), 24);
    
    auto smallFont = fontManager.getFont("small");
    EXPECT_NE(smallFont, nullptr);
    EXPECT_EQ(smallFont->getSize(), 12);
}

TEST_F(FontManagerTest, LoadCustomFontFromAtlasData) {
    FontManager fontManager;
    
    // Create mock font atlas data (16x16 characters, 256x256 texture)
    std::vector<uint8_t> atlasData(256 * 256, 128);  // Gray atlas
    
    // Should be able to load custom font from atlas data
    EXPECT_TRUE(fontManager.loadFontFromAtlas("ExocetBlack", 20, atlasData, 256, 256));
    
    // Font should be available
    EXPECT_TRUE(fontManager.hasFont("ExocetBlack"));
    
    auto customFont = fontManager.getFont("ExocetBlack");
    EXPECT_NE(customFont, nullptr);
    EXPECT_EQ(customFont->getName(), "ExocetBlack");
    EXPECT_EQ(customFont->getSize(), 20);
    EXPECT_GT(customFont->getTextureId(), 0);
}

} // namespace d2