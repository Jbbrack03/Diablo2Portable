#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "ui/font_manager.h"

namespace d2 {

class FontManagerRealImplementationTest : public ::testing::Test {
protected:
    void SetUp() override {
        createTestFontFiles();
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("fonts/default.fnt");
        std::remove("fonts/default.png");
        std::remove("fonts/large.fnt");
        std::remove("fonts/large.png");
        std::remove("fonts/small.fnt");
        std::remove("fonts/small.png");
        std::filesystem::remove("fonts");
    }
    
private:
    void createTestFontFiles() {
        // Create fonts directory
        std::filesystem::create_directory("fonts");
        
        // Create default font (16pt)
        createFontFile("fonts/default.fnt", "DefaultFont", 16, 18);
        createDummyPNG("fonts/default.png");
        
        // Create large font (24pt)
        createFontFile("fonts/large.fnt", "LargeFont", 24, 28);
        createDummyPNG("fonts/large.png");
        
        // Create small font (12pt)
        createFontFile("fonts/small.fnt", "SmallFont", 12, 14);
        createDummyPNG("fonts/small.png");
    }
    
    void createFontFile(const std::string& filename, const std::string& fontName, int size, int lineHeight) {
        std::ofstream fnt(filename);
        fnt << "info face=\"" << fontName << "\" size=" << size << " bold=0 italic=0\n";
        fnt << "common lineHeight=" << lineHeight << " base=" << (lineHeight - 4) << " scaleW=256 scaleH=256 pages=1\n";
        fnt << "page id=0 file=\"" << filename.substr(0, filename.length() - 4) << ".png\"\n";
        fnt << "chars count=1\n";
        fnt << "char id=65 x=0 y=0 width=10 height=14 xoffset=0 yoffset=0 xadvance=11 page=0\n";
        fnt.close();
    }
    
    void createDummyPNG(const std::string& filename) {
        std::ofstream png(filename, std::ios::binary);
        unsigned char pngSig[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        png.write(reinterpret_cast<char*>(pngSig), 8);
        png.close();
    }
};

TEST_F(FontManagerRealImplementationTest, LoadDefaultFontsFromDirectory) {
    FontManager fontManager;
    
    // This should fail with inline implementation
    // Real implementation should scan fonts directory and load .fnt files
    EXPECT_TRUE(fontManager.loadDefaultFontsFromDirectory("fonts"));
    
    // Should have loaded all three fonts
    EXPECT_EQ(fontManager.getFontCount(), 3);
    
    // Fonts should have correct sizes from their .fnt files
    auto defaultFont = fontManager.getFont("default");
    ASSERT_NE(defaultFont, nullptr);
    EXPECT_EQ(defaultFont->getSize(), 16);
    EXPECT_EQ(defaultFont->getLineHeight(), 18);
    
    auto largeFont = fontManager.getFont("large");
    ASSERT_NE(largeFont, nullptr);
    EXPECT_EQ(largeFont->getSize(), 24);
    EXPECT_EQ(largeFont->getLineHeight(), 28);
    
    auto smallFont = fontManager.getFont("small");
    ASSERT_NE(smallFont, nullptr);
    EXPECT_EQ(smallFont->getSize(), 12);
    EXPECT_EQ(smallFont->getLineHeight(), 14);
}

TEST_F(FontManagerRealImplementationTest, CacheFontTextures) {
    FontManager fontManager;
    
    // Load a font twice
    fontManager.loadFontFromFile("default16", "fonts/default.fnt");
    fontManager.loadFontFromFile("default16_again", "fonts/default.fnt");
    
    // Both should share the same texture ID (texture caching)
    auto font1 = fontManager.getFont("default16");
    auto font2 = fontManager.getFont("default16_again");
    
    ASSERT_NE(font1, nullptr);
    ASSERT_NE(font2, nullptr);
    EXPECT_EQ(font1->getTextureId(), font2->getTextureId());
}

TEST_F(FontManagerRealImplementationTest, ReleaseUnusedFonts) {
    FontManager fontManager;
    
    // Create some fonts
    fontManager.createFont("temp1", 16);
    fontManager.createFont("temp2", 16);
    fontManager.createFont("keep", 16);
    
    EXPECT_EQ(fontManager.getFontCount(), 3);
    
    // Mark one as permanent
    fontManager.markFontAsPermanent("keep");
    
    // Release unused fonts
    fontManager.releaseUnusedFonts();
    
    // Only permanent font should remain
    EXPECT_EQ(fontManager.getFontCount(), 1);
    EXPECT_TRUE(fontManager.hasFont("keep"));
    EXPECT_FALSE(fontManager.hasFont("temp1"));
    EXPECT_FALSE(fontManager.hasFont("temp2"));
}

TEST_F(FontManagerRealImplementationTest, FontHotReloading) {
    FontManager fontManager;
    
    // Enable hot reloading
    fontManager.enableHotReloading(true);
    
    // Load a font
    fontManager.loadFontFromFile("test", "fonts/default.fnt");
    auto font = fontManager.getFont("test");
    ASSERT_NE(font, nullptr);
    
    // Get initial advance width
    auto charInfo = font->getCharacterInfo('A');
    EXPECT_EQ(charInfo.advance, 11);
    
    // Modify the font file
    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Ensure different timestamp
    std::ofstream fnt("fonts/default.fnt");
    fnt << "info face=\"DefaultFont\" size=16\n";
    fnt << "common lineHeight=18 base=14 scaleW=256 scaleH=256 pages=1\n";
    fnt << "page id=0 file=\"fonts/default.png\"\n";
    fnt << "chars count=1\n";
    fnt << "char id=65 x=0 y=0 width=10 height=14 xoffset=0 yoffset=0 xadvance=15 page=0\n";  // Changed advance
    fnt.close();
    
    // Check for modifications and reload
    fontManager.checkAndReloadModifiedFonts();
    
    // Font should have new data
    font = fontManager.getFont("test");
    charInfo = font->getCharacterInfo('A');
    EXPECT_EQ(charInfo.advance, 15);  // Updated advance
}

} // namespace d2