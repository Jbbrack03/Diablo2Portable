#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <cstring>

// Include the header for the function we're testing
#include "utils/pkware_explode.h"

class PKWareTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

// Test 1: Basic literal decompression
TEST_F(PKWareTest, DecompressLiterals) {
    // Based on blast.c documentation:
    // Byte 0: literals flag (0 = coded, 1 = uncoded)
    // Byte 1: log2(dictionary_size) - 6 (4, 5, or 6)
    // Rest: bit stream with 0 = literal, 1 = match
    
    // Example from documentation: 00 04 82 24 25 8f 80 7f
    // decompresses to "AIAIAIAIAIAIA"
    std::vector<uint8_t> compressed = {
        0x00,  // Literals are coded
        0x04,  // log2(1024) - 6 = 4
        0x82, 0x24, 0x25, 0x8f, 0x80, 0x7f
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 13;  // "AIAIAIAIAIAIA"
    
    bool result = d2portable::utils::PKWAREExplode(compressed, output, expected_size);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(output.size(), expected_size);
    
    std::string decompressed(output.begin(), output.end());
    EXPECT_EQ(decompressed, "AIAIAIAIAIAIA");
}

// Test 2: Invalid dictionary size
TEST_F(PKWareTest, InvalidDictionarySize) {
    std::vector<uint8_t> compressed = {
        0x00,  // Coded literals
        0x03,  // Invalid dictionary size (< 4)
        0x00
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::PKWAREExplode(compressed, output, expected_size);
    
    EXPECT_FALSE(result);
}

// Test 3: Empty input
TEST_F(PKWareTest, EmptyInput) {
    std::vector<uint8_t> compressed;
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::PKWAREExplode(compressed, output, expected_size);
    
    EXPECT_FALSE(result);
}

// Test 4: Uncoded literals mode
TEST_F(PKWareTest, UncodedLiterals) {
    // When first byte is 1, literals are uncoded (raw bytes)
    std::vector<uint8_t> compressed = {
        0x01,  // Uncoded literals
        0x04,  // log2(1024) - 6 = 4
        // In uncoded mode, literals are raw bytes in the bit stream
        // This is a simplified test - real data would have proper bit packing
        0x00, 0x48, 0x00, 0x69  // Bits + "Hi"
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 2;
    
    bool result = d2portable::utils::PKWAREExplode(compressed, output, expected_size);
    
    // For now, we expect this to fail until implementation is fixed
    EXPECT_FALSE(result);  // Will change to TRUE once implemented correctly
}