#include <gtest/gtest.h>
#include "ui/font.h"

namespace d2 {

class FontTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(FontTest, CreateBasicFont) {
    Font font("Arial", 16);
    
    // Font should have the properties set in constructor
    EXPECT_EQ(font.getName(), "Arial");
    EXPECT_EQ(font.getSize(), 16);
    
    // Default line height should be reasonable
    EXPECT_GT(font.getLineHeight(), 0);
}

TEST_F(FontTest, MeasureTextWidth) {
    Font font("Arial", 16);
    
    // Empty string should have zero width
    EXPECT_EQ(font.getTextWidth(""), 0);
    
    // Single character should have non-zero width
    EXPECT_GT(font.getTextWidth("A"), 0);
    
    // Longer strings should be wider
    int singleCharWidth = font.getTextWidth("A");
    int doubleCharWidth = font.getTextWidth("AA");
    EXPECT_GT(doubleCharWidth, singleCharWidth);
    
    // Basic approximation: each character is roughly font_size/2 wide
    // This is a placeholder until we have real font metrics
    EXPECT_NEAR(font.getTextWidth("Hello"), 5 * font.getSize() / 2, font.getSize());
}

TEST_F(FontTest, LoadFontFromAssetData) {
    Font font("TestFont", 16);
    
    // Create mock bitmap font data (simplified example)
    std::vector<uint8_t> fontAtlasData(256 * 256, 255); // 256x256 white texture
    
    // Font should be able to load from asset data
    EXPECT_TRUE(font.loadFromAtlasData(fontAtlasData, 256, 256));
    
    // After loading, font should have valid texture ID
    EXPECT_GT(font.getTextureId(), 0);
    
    // Character metrics should be available
    auto charInfo = font.getCharacterInfo('A');
    EXPECT_GT(charInfo.width, 0);
    EXPECT_GT(charInfo.height, 0);
    EXPECT_GE(charInfo.u, 0.0f);
    EXPECT_LE(charInfo.u, 1.0f);
    EXPECT_GE(charInfo.v, 0.0f);
    EXPECT_LE(charInfo.v, 1.0f);
}

} // namespace d2