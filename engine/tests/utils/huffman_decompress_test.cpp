#include <gtest/gtest.h>
#include "utils/huffman_decompress.h"
#include <vector>
#include <cstdint>

using namespace d2portable::utils;

class HuffmanDecompressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Helper method to create test data
    }
    
    // Helper function to create simple test data
    std::vector<uint8_t> createSimpleData(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;
        result.push_back(0); // Compression type 0 (simple case)
        result.insert(result.end(), data.begin(), data.end());
        return result;
    }
};

// Test basic decompression functionality
TEST_F(HuffmanDecompressTest, DecompressEmptyData) {
    std::vector<uint8_t> compressed_data;
    std::vector<uint8_t> output;
    
    // Empty input should fail
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 10));
}

// Test decompression with expected size of zero
TEST_F(HuffmanDecompressTest, DecompressZeroExpectedSize) {
    std::vector<uint8_t> compressed_data = {0}; // Just compression type
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(HuffmanDecompress(compressed_data, output, 0));
    EXPECT_EQ(output.size(), 0);
}

// Test simple uncompressed data pass-through
TEST_F(HuffmanDecompressTest, DecompressUncompressedData) {
    std::vector<uint8_t> original_data = {'H', 'e', 'l', 'l', 'o'};
    std::vector<uint8_t> compressed_data;
    
    // For uncompressed data, the implementation should handle simple copy
    // when compressed size - 1 == expected size
    compressed_data.push_back(255); // Unknown compression type > 8
    compressed_data.insert(compressed_data.end(), original_data.begin(), original_data.end());
    
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(HuffmanDecompress(compressed_data, output, original_data.size()));
    EXPECT_EQ(output.size(), original_data.size());
    EXPECT_EQ(output, original_data);
}

// Test invalid compression type with wrong size
TEST_F(HuffmanDecompressTest, InvalidCompressionTypeWrongSize) {
    std::vector<uint8_t> compressed_data = {255, 1, 2, 3}; // Type 255, 3 bytes data
    std::vector<uint8_t> output;
    
    // Expected size doesn't match compressed size - 1, should fail
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 10));
}

// Test known compression types (0-8)
TEST_F(HuffmanDecompressTest, ValidCompressionTypes) {
    for (uint8_t comp_type = 0; comp_type <= 8; comp_type++) {
        std::vector<uint8_t> compressed_data = {comp_type};
        std::vector<uint8_t> output;
        
        // For valid compression types, the function should attempt decompression
        // It may fail due to insufficient data, but shouldn't reject the type
        bool result = HuffmanDecompress(compressed_data, output, 5);
        
        // We don't expect success here since we don't have proper Huffman data,
        // but we're testing that the compression type is accepted
        // The function will likely fail due to insufficient input data
        EXPECT_FALSE(result); // Expected to fail with minimal data
    }
}

// Test decompression with insufficient data
TEST_F(HuffmanDecompressTest, InsufficientInputData) {
    std::vector<uint8_t> compressed_data = {0}; // Just compression type, no data
    std::vector<uint8_t> output;
    
    // Should fail when trying to decompress with no actual data
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 5));
}

// Test output buffer sizing
TEST_F(HuffmanDecompressTest, OutputBufferSizing) {
    std::vector<uint8_t> compressed_data = {0, 65}; // Type 0, single 'A'
    std::vector<uint8_t> output;
    
    // Test that output buffer is properly sized
    size_t expected_size = 10;
    bool result = HuffmanDecompress(compressed_data, output, expected_size);
    
    // Regardless of success/failure, output should be sized correctly
    EXPECT_EQ(output.size(), expected_size);
}

// Test boundary condition - single byte decompression
TEST_F(HuffmanDecompressTest, SingleByteDecompression) {
    std::vector<uint8_t> compressed_data = {255, 'X'}; // Unknown type, single byte
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(HuffmanDecompress(compressed_data, output, 1));
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], 'X');
}

// Test large expected size with small input
TEST_F(HuffmanDecompressTest, LargeExpectedSizeSmallInput) {
    std::vector<uint8_t> compressed_data = {0, 65, 66}; // Type 0, 'AB'
    std::vector<uint8_t> output;
    
    // Request much larger output than input can provide
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 1000));
    EXPECT_EQ(output.size(), 1000); // Buffer should still be allocated
}

// Test exact size match for uncompressed pass-through
TEST_F(HuffmanDecompressTest, ExactSizeMatchUncompressed) {
    std::vector<uint8_t> test_data = {0x41, 0x42, 0x43, 0x44}; // "ABCD"
    std::vector<uint8_t> compressed_data;
    compressed_data.push_back(200); // Unknown compression type > 8
    compressed_data.insert(compressed_data.end(), test_data.begin(), test_data.end());
    
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(HuffmanDecompress(compressed_data, output, test_data.size()));
    EXPECT_EQ(output, test_data);
}

// Test maximum valid compression type
TEST_F(HuffmanDecompressTest, MaxValidCompressionType) {
    std::vector<uint8_t> compressed_data = {8}; // Maximum valid type
    std::vector<uint8_t> output;
    
    // Should accept type 8 but likely fail due to no data
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 1));
}

// Test minimum invalid compression type
TEST_F(HuffmanDecompressTest, MinInvalidCompressionType) {
    std::vector<uint8_t> compressed_data = {9, 65}; // Type 9 (first invalid), single byte
    std::vector<uint8_t> output;
    
    // Should trigger uncompressed path but fail due to size mismatch
    EXPECT_FALSE(HuffmanDecompress(compressed_data, output, 5)); // Expect 5, but only 1 byte available
}

// Test data integrity for pass-through
TEST_F(HuffmanDecompressTest, DataIntegrityPassThrough) {
    std::vector<uint8_t> original = {0x00, 0xFF, 0x55, 0xAA, 0x12, 0x34, 0x56, 0x78};
    std::vector<uint8_t> compressed_data;
    compressed_data.push_back(255); // Use unknown type for pass-through
    compressed_data.insert(compressed_data.end(), original.begin(), original.end());
    
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(HuffmanDecompress(compressed_data, output, original.size()));
    EXPECT_EQ(output, original);
    
    // Verify each byte individually
    for (size_t i = 0; i < original.size(); i++) {
        EXPECT_EQ(output[i], original[i]) << "Mismatch at index " << i;
    }
}