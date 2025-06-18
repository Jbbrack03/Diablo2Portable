#include <gtest/gtest.h>
#include "utils/sparse_decompress.h"
#include <vector>
#include <cstdint>

using namespace d2portable::utils;

class SparseDecompressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
};

// Test basic empty input validation
TEST_F(SparseDecompressTest, DecompressEmptyData) {
    std::vector<uint8_t> compressed_data;
    std::vector<uint8_t> output;
    
    EXPECT_FALSE(SparseDecompress(compressed_data, output, 10));
    EXPECT_TRUE(output.empty());
}

// Test zero expected size
TEST_F(SparseDecompressTest, DecompressZeroExpectedSize) {
    std::vector<uint8_t> compressed_data = {1, 65}; // Run of 1 'A'
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 0));
    EXPECT_EQ(output.size(), 0);
}

// Test literal byte copying (run_length = 0)
TEST_F(SparseDecompressTest, LiteralByteCopying) {
    std::vector<uint8_t> compressed_data = {
        0, 'A',  // Literal 'A'
        0, 'B',  // Literal 'B'
        0, 'C'   // Literal 'C'
    };
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 3));
    EXPECT_EQ(output.size(), 3);
    EXPECT_EQ(output[0], 'A');
    EXPECT_EQ(output[1], 'B');
    EXPECT_EQ(output[2], 'C');
}

// Test simple run-length encoding
TEST_F(SparseDecompressTest, SimpleRunLengthEncoding) {
    std::vector<uint8_t> compressed_data = {5, 'X'}; // Run of 5 'X's
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 5);
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(output[i], 'X');
    }
}

// Test maximum run length (255)
TEST_F(SparseDecompressTest, MaximumRunLength) {
    std::vector<uint8_t> compressed_data = {255, 'Z'}; // Run of 255 'Z's
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 255));
    EXPECT_EQ(output.size(), 255);
    for (int i = 0; i < 255; i++) {
        EXPECT_EQ(output[i], 'Z');
    }
}

// Test mixed literal and run-length data
TEST_F(SparseDecompressTest, MixedLiteralAndRunLength) {
    std::vector<uint8_t> compressed_data = {
        0, 'A',    // Literal 'A'
        3, 'B',    // Run of 3 'B's
        0, 'C',    // Literal 'C'
        2, 'D'     // Run of 2 'D's
    };
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 7));
    EXPECT_EQ(output.size(), 7);
    
    std::vector<uint8_t> expected = {'A', 'B', 'B', 'B', 'C', 'D', 'D'};
    EXPECT_EQ(output, expected);
}

// Test incomplete data (missing value byte)
TEST_F(SparseDecompressTest, IncompleteData) {
    std::vector<uint8_t> compressed_data = {5}; // Run length but no value byte
    std::vector<uint8_t> output;
    
    EXPECT_FALSE(SparseDecompress(compressed_data, output, 5));
}

// Test truncated run due to expected size limit
TEST_F(SparseDecompressTest, TruncatedRunDueToSizeLimit) {
    std::vector<uint8_t> compressed_data = {10, 'X'}; // Run of 10 'X's
    std::vector<uint8_t> output;
    
    // But only expect 5 bytes
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 5);
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(output[i], 'X');
    }
}

// Test exact size match
TEST_F(SparseDecompressTest, ExactSizeMatch) {
    std::vector<uint8_t> compressed_data = {
        2, 'A',    // 2 'A's
        3, 'B'     // 3 'B's
    };
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 5);
    
    std::vector<uint8_t> expected = {'A', 'A', 'B', 'B', 'B'};
    EXPECT_EQ(output, expected);
}

// Test size mismatch (too much data)
TEST_F(SparseDecompressTest, SizeMismatchTooMuchData) {
    std::vector<uint8_t> compressed_data = {
        5, 'A',    // 5 'A's
        3, 'B'     // 3 'B's  (total would be 8)
    };
    std::vector<uint8_t> output;
    
    // Expect only 5 bytes
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 5);
    
    // Should be 5 'A's (stopped at expected size)
    std::vector<uint8_t> expected = {'A', 'A', 'A', 'A', 'A'};
    EXPECT_EQ(output, expected);
}

// Test size mismatch (too little data)
TEST_F(SparseDecompressTest, SizeMismatchTooLittleData) {
    std::vector<uint8_t> compressed_data = {2, 'A'}; // Only 2 'A's
    std::vector<uint8_t> output;
    
    // Expect 5 bytes
    EXPECT_FALSE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 2); // Only got 2 bytes
}

// Test single byte operations
TEST_F(SparseDecompressTest, SingleByteOperations) {
    std::vector<uint8_t> compressed_data = {1, 'X'}; // Single 'X'
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 1));
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], 'X');
}

// Test binary data (non-printable bytes)
TEST_F(SparseDecompressTest, BinaryData) {
    std::vector<uint8_t> compressed_data = {
        0, 0x00,    // Literal null byte
        3, 0xFF,    // Run of 3 0xFF bytes
        0, 0x55,    // Literal 0x55
        2, 0xAA     // Run of 2 0xAA bytes
    };
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 7));
    EXPECT_EQ(output.size(), 7);
    
    std::vector<uint8_t> expected = {0x00, 0xFF, 0xFF, 0xFF, 0x55, 0xAA, 0xAA};
    EXPECT_EQ(output, expected);
}

// Test edge case: run length 1
TEST_F(SparseDecompressTest, RunLengthOne) {
    std::vector<uint8_t> compressed_data = {1, 'Y'}; // Run of 1 'Y'
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 1));
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], 'Y');
}

// Test multiple consecutive literals
TEST_F(SparseDecompressTest, ConsecutiveLiterals) {
    std::vector<uint8_t> compressed_data = {
        0, 'H',
        0, 'e',
        0, 'l',
        0, 'l',
        0, 'o'
    };
    std::vector<uint8_t> output;
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 5);
    
    std::string result(output.begin(), output.end());
    EXPECT_EQ(result, "Hello");
}

// Test output buffer behavior (clearing)
TEST_F(SparseDecompressTest, OutputBufferClearing) {
    std::vector<uint8_t> compressed_data = {2, 'A'};
    std::vector<uint8_t> output = {'X', 'Y', 'Z'}; // Pre-fill output
    
    EXPECT_TRUE(SparseDecompress(compressed_data, output, 2));
    EXPECT_EQ(output.size(), 2);
    EXPECT_EQ(output[0], 'A');
    EXPECT_EQ(output[1], 'A');
    // Previous content should be cleared
}

// Test early termination on input exhaustion
TEST_F(SparseDecompressTest, EarlyTerminationInputExhaustion) {
    std::vector<uint8_t> compressed_data = {
        2, 'A',
        0  // Run length but no value byte (truncated)
    };
    std::vector<uint8_t> output;
    
    EXPECT_FALSE(SparseDecompress(compressed_data, output, 5));
    EXPECT_EQ(output.size(), 2); // Should have 2 'A's before termination
}