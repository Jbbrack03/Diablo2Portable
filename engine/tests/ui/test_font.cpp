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

} // namespace d2