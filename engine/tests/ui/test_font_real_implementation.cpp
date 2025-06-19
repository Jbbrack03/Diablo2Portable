#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include "ui/font.h"

namespace d2 {

class FontRealImplementationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test font data file (bitmap font format)
        createTestBitmapFont();
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("test_font.fnt");
        std::remove("test_font.png");
    }
    
private:
    void createTestBitmapFont() {
        // Create a simple bitmap font file (.fnt format)
        std::ofstream fnt("test_font.fnt");
        fnt << "info face=\"TestFont\" size=16 bold=0 italic=0 charset=\"\" unicode=1 "
            << "stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1 outline=0\n";
        fnt << "common lineHeight=18 base=14 scaleW=256 scaleH=256 pages=1 packed=0\n";
        fnt << "page id=0 file=\"test_font.png\"\n";
        fnt << "chars count=3\n";
        // Character 'A' (ASCII 65)
        fnt << "char id=65  x=0   y=0   width=8  height=14 xoffset=0  yoffset=0  xadvance=9  page=0  chnl=15\n";
        // Character 'B' (ASCII 66)
        fnt << "char id=66  x=8   y=0   width=8  height=14 xoffset=0  yoffset=0  xadvance=9  page=0  chnl=15\n";
        // Character 'C' (ASCII 67)
        fnt << "char id=67  x=16  y=0   width=8  height=14 xoffset=0  yoffset=0  xadvance=9  page=0  chnl=15\n";
        fnt.close();
        
        // Create a dummy PNG file (just headers for now)
        std::ofstream png("test_font.png", std::ios::binary);
        // PNG signature
        unsigned char pngSig[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        png.write(reinterpret_cast<char*>(pngSig), 8);
        png.close();
    }
};

TEST_F(FontRealImplementationTest, LoadBitmapFontFile) {
    Font font("TestFont", 16);
    
    // This test should fail with the current inline implementation
    // because it doesn't actually load font files
    EXPECT_TRUE(font.loadFromFile("test_font.fnt"));
    
    // After loading, font should have accurate metrics for loaded characters
    auto charA = font.getCharacterInfo('A');
    EXPECT_EQ(charA.width, 8);
    EXPECT_EQ(charA.height, 14);
    EXPECT_EQ(charA.advance, 9);
    
    // Character that wasn't loaded should use fallback
    auto charZ = font.getCharacterInfo('Z');
    EXPECT_NE(charZ.width, 8);  // Should be different from loaded chars
}

TEST_F(FontRealImplementationTest, AccurateTextWidth) {
    Font font("TestFont", 16);
    font.loadFromFile("test_font.fnt");
    
    // With real font metrics, text width should be accurate
    // "AAA" = 3 * 9 (advance) = 27
    EXPECT_EQ(font.getTextWidth("AAA"), 27);
    
    // "ABC" = 9 + 9 + 9 = 27
    EXPECT_EQ(font.getTextWidth("ABC"), 27);
    
    // Mixed with unloaded character
    // "ABZ" - Z should use fallback width
    int width = font.getTextWidth("ABZ");
    EXPECT_NE(width, 27);  // Should be different due to fallback char
}

TEST_F(FontRealImplementationTest, KerningSupport) {
    Font font("TestFont", 16);
    
    // Create a font file with kerning data
    std::ofstream fnt("test_font_kerning.fnt");
    fnt << "info face=\"TestFont\" size=16\n";
    fnt << "common lineHeight=18 base=14 scaleW=256 scaleH=256 pages=1\n";
    fnt << "page id=0 file=\"test_font.png\"\n";
    fnt << "chars count=2\n";
    fnt << "char id=65 x=0 y=0 width=8 height=14 xoffset=0 yoffset=0 xadvance=9 page=0\n";
    fnt << "char id=86 x=8 y=0 width=8 height=14 xoffset=0 yoffset=0 xadvance=9 page=0\n";
    fnt << "kernings count=1\n";
    fnt << "kerning first=65 second=86 amount=-2\n";  // AV kerning pair
    fnt.close();
    
    font.loadFromFile("test_font_kerning.fnt");
    
    // "AV" with kerning should be less than 2 * advance
    int avWidth = font.getTextWidth("AV");
    EXPECT_EQ(avWidth, 16);  // 9 + 9 - 2 = 16
    
    // "AA" without kerning should be normal
    int aaWidth = font.getTextWidth("AA");
    EXPECT_EQ(aaWidth, 18);  // 9 + 9 = 18
    
    std::remove("test_font_kerning.fnt");
}

TEST_F(FontRealImplementationTest, CreateFontAtlas) {
    Font font("TestFont", 16);
    font.loadFromFile("test_font.fnt");
    
    // Font should create an OpenGL texture atlas
    EXPECT_GT(font.getTextureId(), 0);
    
    // Atlas dimensions should match font file specifications
    EXPECT_EQ(font.getAtlasWidth(), 256);
    EXPECT_EQ(font.getAtlasHeight(), 256);
}

} // namespace d2