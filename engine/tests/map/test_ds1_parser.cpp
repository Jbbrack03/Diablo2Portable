#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <cstdint>
#include "map/ds1_parser.h"

class DS1ParserTest : public ::testing::Test {
protected:
    d2::map::DS1Parser parser;
    
    // Helper to create a minimal DS1 file buffer
    std::vector<uint8_t> createMinimalDS1() {
        std::vector<uint8_t> data;
        
        // DS1 Header (simplified version based on research)
        // Version (4 bytes)
        data.push_back(0x12); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // Version 18
        
        // Width + 1 (4 bytes)
        data.push_back(0x0A); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 10
        
        // Height + 1 (4 bytes)
        data.push_back(0x0A); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 10
        
        // Act (4 bytes) - 1-based (Act 1 = 1)
        data.push_back(0x01); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // Act 1
        
        // Layer type (4 bytes)
        data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // Floor
        
        // Number of walls (4 bytes)
        data.push_back(0x01); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 1 wall layer
        
        // Number of floors (4 bytes)
        data.push_back(0x01); data.push_back(0x00); data.push_back(0x00); data.push_back(0x00); // 1 floor layer
        
        return data;
    }
};

// Test 1: Parse DS1 header version
TEST_F(DS1ParserTest, ParseHeaderVersion) {
    auto data = createMinimalDS1();
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(result->getVersion(), 18);  // Version 18 is common for DS1 files
}

// Test 2: Parse map dimensions
TEST_F(DS1ParserTest, ParseMapDimensions) {
    auto data = createMinimalDS1();
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    // DS1 stores width/height + 1
    EXPECT_EQ(result->getWidth(), 9);   // 10 - 1
    EXPECT_EQ(result->getHeight(), 9);  // 10 - 1
}

// Test 3: Parse act information
TEST_F(DS1ParserTest, ParseActInformation) {
    auto data = createMinimalDS1();
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(result->getAct(), 1);  // Act 1
}

// Test 4: Parse layer counts
TEST_F(DS1ParserTest, ParseLayerCounts) {
    auto data = createMinimalDS1();
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(result->getWallLayerCount(), 1);
    EXPECT_EQ(result->getFloorLayerCount(), 1);
}

// Test 5: Parse floor layer data
TEST_F(DS1ParserTest, ParseFloorLayerData) {
    auto data = createMinimalDS1();
    
    // Add floor layer data (simplified)
    // For each tile: prop1, prop2, prop3, prop4, orientation, main_index, sub_index, unknown
    // Note: DS1 format stores width-1 and height-1, so 10 becomes 9, giving us 9x9=81 tiles
    for (int i = 0; i < 81; i++) {  // 9x9 tiles
        // Prop values (4 bytes each)
        for (int j = 0; j < 4; j++) {
            data.push_back(0x00); data.push_back(0x00); 
            data.push_back(0x00); data.push_back(0x00);
        }
        // Orientation (1 byte)
        data.push_back(0x00);
        // Main index (4 bytes)
        data.push_back(0x01); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
        // Sub index (4 bytes)
        data.push_back(0x00); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
        // Unknown (4 bytes)
        data.push_back(0x00); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
    }
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    auto floorLayer = result->getFloorLayer(0);
    ASSERT_TRUE(floorLayer != nullptr);
    
    // Check a specific tile
    auto tile = floorLayer->getTile(0, 0);
    EXPECT_EQ(tile.orientation, 0);
    EXPECT_EQ(tile.mainIndex, 1);
    EXPECT_EQ(tile.subIndex, 0);
}

// Test 6: Parse wall layer data
TEST_F(DS1ParserTest, ParseWallLayerData) {
    auto data = createMinimalDS1();
    
    // Add floor layer data first (81 tiles)
    for (int i = 0; i < 81 * 29; i++) {
        data.push_back(0x00);
    }
    
    // Add wall layer data
    for (int i = 0; i < 81; i++) {  // 9x9 tiles
        // Prop values (4 bytes each)
        for (int j = 0; j < 4; j++) {
            data.push_back(0x00); data.push_back(0x00); 
            data.push_back(0x00); data.push_back(0x00);
        }
        // Orientation (1 byte)
        data.push_back(0x0F);  // Wall orientation
        // Main index (4 bytes)
        data.push_back(0x02); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
        // Sub index (4 bytes)
        data.push_back(0x01); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
        // Unknown (4 bytes)
        data.push_back(0x00); data.push_back(0x00); 
        data.push_back(0x00); data.push_back(0x00);
    }
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    auto wallLayer = result->getWallLayer(0);
    ASSERT_TRUE(wallLayer != nullptr);
    
    // Check a specific tile
    auto tile = wallLayer->getTile(0, 0);
    EXPECT_EQ(tile.orientation, 15);  // Wall orientation
    EXPECT_EQ(tile.mainIndex, 2);
    EXPECT_EQ(tile.subIndex, 1);
}

// Test 7: Parse shadow layer presence
TEST_F(DS1ParserTest, ParseShadowLayer) {
    auto data = createMinimalDS1();
    
    // Modify header to indicate shadow layer
    data[16] = 0x01;  // Set layer type to include shadow (at offset 16)
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_TRUE(result->hasShadowLayer());
}

// Test 8: Handle invalid version
TEST_F(DS1ParserTest, HandleInvalidVersion) {
    auto data = createMinimalDS1();
    
    // Set invalid version
    data[0] = 0xFF; data[1] = 0xFF;
    
    auto result = parser.parse(data);
    
    EXPECT_EQ(result, nullptr);  // Should fail to parse
}

// Test 9: Parse multiple floor layers
TEST_F(DS1ParserTest, ParseMultipleFloorLayers) {
    auto data = createMinimalDS1();
    
    // Set 4 floor layers
    data[24] = 0x04;
    
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(result->getFloorLayerCount(), 4);
    
    // Verify all 4 layers exist
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(result->getFloorLayer(i) != nullptr);
    }
}

// Test 10: Load DS1 from file path
TEST_F(DS1ParserTest, LoadFromFile) {
    // This test assumes we'll have a test DS1 file
    const std::string testFile = "test_data/minimal.ds1";
    
    auto result = parser.loadFromFile(testFile);
    
    // Will fail until we have actual test files
    EXPECT_TRUE(result != nullptr);
}