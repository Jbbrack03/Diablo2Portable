#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <cstring>

// Forward declaration of the function we're testing
namespace d2portable {
namespace utils {
    bool bzip2_decompress(const std::vector<uint8_t>& compressed_data, 
                         std::vector<uint8_t>& output, 
                         size_t expected_size);
}
}

class BZip2Test : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

// Test 1: Decompress simple BZip2 data
TEST_F(BZip2Test, DecompressSimpleData) {
    // BZip2 compressed "Hello, World!"
    // Generated with: echo -n "Hello, World!" | bzip2 -c | od -An -tx1
    std::vector<uint8_t> compressed = {
        0x42, 0x5a, 0x68, 0x39,  // BZh9 header
        0x31, 0x41, 0x59, 0x26,  // BZ signature
        0x53, 0x59, 0xe6, 0xd8,
        0xfe, 0xdf, 0x00, 0x00,
        0x01, 0x97, 0x80, 0x60,
        0x04, 0x00, 0x40, 0x00,
        0x80, 0x06, 0x04, 0x90,
        0x00, 0x20, 0x00, 0x22,
        0x03, 0x23, 0x21, 0x00,
        0x30, 0xb2, 0x80, 0x5a,
        0xde, 0x43, 0xef, 0x17,
        0x72, 0x45, 0x38, 0x50,
        0x90, 0xe6, 0xd8, 0xfe,
        0xdf
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 13;  // "Hello, World!"
    
    bool result = d2portable::utils::bzip2_decompress(compressed, output, expected_size);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(output.size(), expected_size);
    
    std::string decompressed(output.begin(), output.end());
    EXPECT_EQ(decompressed, "Hello, World!");
}

// Test 2: Invalid BZip2 header
TEST_F(BZip2Test, InvalidHeader) {
    std::vector<uint8_t> compressed = {
        0x00, 0x00, 0x00, 0x00  // Not a BZip2 header
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::bzip2_decompress(compressed, output, expected_size);
    
    EXPECT_FALSE(result);
}

// Test 3: Empty input
TEST_F(BZip2Test, EmptyInput) {
    std::vector<uint8_t> compressed;
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::bzip2_decompress(compressed, output, expected_size);
    
    EXPECT_FALSE(result);
}

// Test 4: Decompress larger data
TEST_F(BZip2Test, DecompressLargerData) {
    // BZip2 compressed repetitive data "AAAAAAAAAA" (10 A's)
    // Generated with: echo -n "AAAAAAAAAA" | bzip2 -c | od -An -tx1
    std::vector<uint8_t> compressed = {
        0x42, 0x5a, 0x68, 0x39,  // BZh9 header
        0x31, 0x41, 0x59, 0x26,  // BZ signature
        0x53, 0x59, 0xa2, 0xf8,
        0x4f, 0x0e, 0x00, 0x00,
        0x02, 0x44, 0x00, 0x01,
        0x00, 0x20, 0x00, 0x20,
        0x00, 0x21, 0x00, 0x82,
        0x0b, 0x17, 0x72, 0x45,
        0x38, 0x50, 0x90, 0xa2,
        0xf8, 0x4f, 0x0e
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::bzip2_decompress(compressed, output, expected_size);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(output.size(), expected_size);
    
    // Check all bytes are 'A'
    for (size_t i = 0; i < output.size(); i++) {
        EXPECT_EQ(output[i], 'A');
    }
}

// Test 5: Truncated compressed data
TEST_F(BZip2Test, TruncatedData) {
    // Start of valid BZip2 data but truncated
    std::vector<uint8_t> compressed = {
        0x42, 0x5a, 0x68, 0x39,  // BZh9 header
        0x31, 0x41, 0x59, 0x26   // Incomplete
    };
    
    std::vector<uint8_t> output;
    size_t expected_size = 10;
    
    bool result = d2portable::utils::bzip2_decompress(compressed, output, expected_size);
    
    EXPECT_FALSE(result);
}