#include <gtest/gtest.h>
#include "core/asset_manager.h"
#include "utils/mock_mpq_builder.h"
#include <filesystem>
#include <vector>

using namespace d2portable::core;
using namespace d2portable::utils;

// Integration tests using mock MPQ files to avoid requiring real game files
class MockMPQIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory for mock MPQ files
        test_dir = std::filesystem::temp_directory_path() / "d2portable_mock_mpq_test";
        std::filesystem::create_directories(test_dir);
        
        mock_mpq_path = test_dir / "mock_d2data.mpq";
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }
    
    std::filesystem::path test_dir;
    std::filesystem::path mock_mpq_path;
    AssetManager asset_manager;
};

// Test 1: Create and load a mock MPQ file
TEST_F(MockMPQIntegrationTest, CreateAndLoadMockMPQ) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Add some test files to the mock MPQ
    std::vector<uint8_t> test_data = {'T', 'E', 'S', 'T'};
    builder.addFile("data\\test\\file1.txt", test_data);
    
    // Add a mock DC6 sprite (minimal valid DC6 header)
    std::vector<uint8_t> dc6_header(24, 0);
    // DC6 version (6)
    *reinterpret_cast<uint32_t*>(&dc6_header[0]) = 6;
    // Flags
    *reinterpret_cast<uint32_t*>(&dc6_header[4]) = 0;
    // Encoding
    *reinterpret_cast<uint32_t*>(&dc6_header[8]) = 0;
    // Termination
    *reinterpret_cast<uint32_t*>(&dc6_header[12]) = 0xEEEEEEEE;
    // Directions (1)
    *reinterpret_cast<uint32_t*>(&dc6_header[16]) = 1;
    // Frames per direction (1)
    *reinterpret_cast<uint32_t*>(&dc6_header[20]) = 1;
    
    builder.addFile("data\\global\\ui\\cursor\\test.dc6", dc6_header);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Now try to load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Verify we can access the files
    EXPECT_TRUE(asset_manager.hasFile("data\\test\\file1.txt"));
    EXPECT_TRUE(asset_manager.hasFile("data\\global\\ui\\cursor\\test.dc6"));
}

// Test 2: Extract and validate DC6 sprite from mock MPQ
TEST_F(MockMPQIntegrationTest, ExtractDC6FromMockMPQ) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Create a more complete DC6 file with direction and frame data
    std::vector<uint8_t> dc6_data;
    
    // DC6 Header (24 bytes)
    // Version (6)
    dc6_data.insert(dc6_data.end(), {0x06, 0x00, 0x00, 0x00});
    // Flags (1 = has direction data)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    // Encoding (0 = uncompressed)
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Termination bytes
    dc6_data.insert(dc6_data.end(), {0xEE, 0xEE, 0xEE, 0xEE});
    // Directions (1)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    // Frames per direction (1)
    dc6_data.insert(dc6_data.end(), {0x01, 0x00, 0x00, 0x00});
    
    // Frame pointer (follows header)
    // For 1 direction, 1 frame per direction, we have 1 frame pointer
    uint32_t frame_offset = 24 + 4; // Header + one frame pointer
    dc6_data.insert(dc6_data.end(), {
        static_cast<uint8_t>(frame_offset & 0xFF),
        static_cast<uint8_t>((frame_offset >> 8) & 0xFF),
        static_cast<uint8_t>((frame_offset >> 16) & 0xFF),
        static_cast<uint8_t>((frame_offset >> 24) & 0xFF)
    });
    
    
    // Frame header (32 bytes)
    // Flip
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Width
    dc6_data.insert(dc6_data.end(), {0x10, 0x00, 0x00, 0x00}); // 16 pixels
    // Height
    dc6_data.insert(dc6_data.end(), {0x10, 0x00, 0x00, 0x00}); // 16 pixels
    // OffsetX
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // OffsetY
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Unknown
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // NextBlock
    dc6_data.insert(dc6_data.end(), {0x00, 0x00, 0x00, 0x00});
    // Length
    dc6_data.insert(dc6_data.end(), {0x00, 0x01, 0x00, 0x00}); // 256 bytes of pixel data
    
    // Add some pixel data (16x16 = 256 bytes)
    for (int i = 0; i < 256; i++) {
        dc6_data.push_back(static_cast<uint8_t>(i & 0xFF));
    }
    
    builder.addFile("data\\global\\ui\\cursor\\hand.dc6", dc6_data);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Now try to load the DC6 sprite
    auto sprite = asset_manager.loadSprite("data\\global\\ui\\cursor\\hand.dc6");
    ASSERT_NE(sprite, nullptr);
    
    // Validate the sprite properties
    EXPECT_EQ(sprite->getDirectionCount(), 1);
    EXPECT_EQ(sprite->getFramesPerDirection(), 1);
    
    // Get frame data
    auto frame = sprite->getFrame(0, 0);
    EXPECT_EQ(frame.width, 16);
    EXPECT_EQ(frame.height, 16);
}

// Test 3: Validate compression type handling with mock MPQ
TEST_F(MockMPQIntegrationTest, ValidateCompressionTypes) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Add an uncompressed text file (without null terminator for exact string comparison)
    std::vector<uint8_t> text_data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    builder.addFile("data\\global\\excel\\test.txt", text_data);
    
    // Add a simple binary file that would normally be compressed
    std::vector<uint8_t> binary_data(1024, 0xAA); // 1KB of repeated data - good for compression
    builder.addFile("data\\global\\test\\binary.dat", binary_data);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Extract the text file and verify content
    auto text_content = asset_manager.loadFileData("data\\global\\excel\\test.txt");
    ASSERT_FALSE(text_content.empty());
    std::string extracted_text(text_content.begin(), text_content.end());
    EXPECT_EQ(extracted_text, "Hello World!");
    
    // Extract the binary file and verify size
    auto binary_content = asset_manager.loadFileData("data\\global\\test\\binary.dat");
    ASSERT_FALSE(binary_content.empty());
    EXPECT_EQ(binary_content.size(), 1024);
    
    // Verify that all bytes in binary content are correct
    for (size_t i = 0; i < binary_content.size(); i++) {
        EXPECT_EQ(binary_content[i], 0xAA) << "Byte at index " << i << " is incorrect";
    }
}

// Test 4: PKWARE format analysis with mock compressed data
TEST_F(MockMPQIntegrationTest, AnalyzePKWARECompressedData) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Create test data that would benefit from PKWARE compression
    // PKWARE works well with repetitive patterns and text
    std::vector<uint8_t> pkware_test_data;
    std::string repeated_text = "This is a test string for PKWARE compression analysis. ";
    
    // Repeat the text 20 times to create a pattern good for compression
    for (int i = 0; i < 20; i++) {
        pkware_test_data.insert(pkware_test_data.end(), 
                               repeated_text.begin(), repeated_text.end());
    }
    
    builder.addFile("data\\global\\excel\\compressed.txt", pkware_test_data);
    
    // Build the mock MPQ file with compression enabled
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Extract the file
    auto extracted_data = asset_manager.loadFileData("data\\global\\excel\\compressed.txt");
    ASSERT_FALSE(extracted_data.empty());
    
    // Verify the extracted data matches original
    EXPECT_EQ(extracted_data.size(), pkware_test_data.size());
    
    // Convert extracted data to string for comparison
    std::string extracted_text(extracted_data.begin(), extracted_data.end());
    std::string original_text(pkware_test_data.begin(), pkware_test_data.end());
    EXPECT_EQ(extracted_text, original_text);
    
    // Verify the repetitive pattern is correctly restored
    EXPECT_TRUE(extracted_text.find("This is a test string for PKWARE compression analysis.") != std::string::npos);
    
    // Count occurrences of the pattern to ensure decompression worked correctly
    size_t pattern_count = 0;
    size_t pos = 0;
    while ((pos = extracted_text.find("This is a test string", pos)) != std::string::npos) {
        pattern_count++;
        pos += 1;
    }
    EXPECT_EQ(pattern_count, 20) << "Expected 20 occurrences of the pattern in decompressed data";
}

// Test 5: PKWARE decompression validation with specific compression scenarios
TEST_F(MockMPQIntegrationTest, ValidatePKWAREDecompressionProcess) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Create test data with different patterns that exercise PKWARE decompression
    
    // Pattern 1: Dictionary references (repeated words)
    std::vector<uint8_t> dictionary_test;
    std::string base_word = "DICTIONARY_REFERENCE_TEST ";
    for (int i = 0; i < 15; i++) {
        dictionary_test.insert(dictionary_test.end(), base_word.begin(), base_word.end());
    }
    builder.addFile("data\\\\global\\\\test\\\\dictionary.txt", dictionary_test);
    
    // Pattern 2: Distance/length pairs (sequential repetition)
    std::vector<uint8_t> distance_test;
    std::string sequence = "ABCDEFGHIJ";
    // Create pattern where PKWARE can use distance/length pairs
    for (int i = 0; i < 10; i++) {
        distance_test.insert(distance_test.end(), sequence.begin(), sequence.end());
        sequence += "K"; // Slight variation to test different distance patterns
    }
    builder.addFile("data\\\\global\\\\test\\\\distance_pairs.txt", distance_test);
    
    // Pattern 3: Huffman coding test (character frequency patterns)
    std::vector<uint8_t> huffman_test;
    // Create data with specific character frequency for Huffman optimization
    std::string frequent_chars = "AAAAAAAA"; // High frequency
    std::string medium_chars = "BBBB";       // Medium frequency
    std::string rare_chars = "C";            // Low frequency
    for (int i = 0; i < 20; i++) {
        huffman_test.insert(huffman_test.end(), frequent_chars.begin(), frequent_chars.end());
        huffman_test.insert(huffman_test.end(), medium_chars.begin(), medium_chars.end());
        huffman_test.insert(huffman_test.end(), rare_chars.begin(), rare_chars.end());
    }
    builder.addFile("data\\\\global\\\\test\\\\huffman_pattern.txt", huffman_test);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Test 1: Dictionary reference decompression
    auto dict_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\dictionary.txt");
    ASSERT_FALSE(dict_result.empty());
    std::string dict_content(dict_result.begin(), dict_result.end());
    EXPECT_EQ(dict_content.size(), dictionary_test.size());
    // Verify the pattern is correctly reconstructed
    EXPECT_TRUE(dict_content.find("DICTIONARY_REFERENCE_TEST") != std::string::npos);
    size_t dict_count = 0;
    size_t dict_pos = 0;
    while ((dict_pos = dict_content.find("DICTIONARY_REFERENCE_TEST", dict_pos)) != std::string::npos) {
        dict_count++;
        dict_pos += 1;
    }
    EXPECT_EQ(dict_count, 15) << "Dictionary reference pattern not correctly decompressed";
    
    // Test 2: Distance/length pair decompression
    auto distance_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\distance_pairs.txt");
    ASSERT_FALSE(distance_result.empty());
    std::string distance_content(distance_result.begin(), distance_result.end());
    EXPECT_EQ(distance_content.size(), distance_test.size());
    // Verify sequential patterns are correctly decompressed
    EXPECT_TRUE(distance_content.find("ABCDEFGHIJ") != std::string::npos);
    EXPECT_TRUE(distance_content.find("ABCDEFGHIJK") != std::string::npos);
    
    // Test 3: Huffman coding decompression
    auto huffman_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\huffman_pattern.txt");
    ASSERT_FALSE(huffman_result.empty());
    std::string huffman_content(huffman_result.begin(), huffman_result.end());
    EXPECT_EQ(huffman_content.size(), huffman_test.size());
    // Verify character frequencies are preserved
    size_t a_count = std::count(huffman_content.begin(), huffman_content.end(), 'A');
    size_t b_count = std::count(huffman_content.begin(), huffman_content.end(), 'B');
    size_t c_count = std::count(huffman_content.begin(), huffman_content.end(), 'C');
    EXPECT_EQ(a_count, 160) << "High-frequency character 'A' count incorrect after decompression";
    EXPECT_EQ(b_count, 80) << "Medium-frequency character 'B' count incorrect after decompression";
    EXPECT_EQ(c_count, 20) << "Low-frequency character 'C' count incorrect after decompression";
}

// Test 6: Compression algorithm validation with different compression scenarios
TEST_F(MockMPQIntegrationTest, ValidateCompressionAlgorithmSupport) {
    // Create a mock MPQ builder
    MockMPQBuilder builder;
    
    // Test different types of data that would benefit from different compression algorithms
    
    // Type 1: Highly compressible text data (good for any algorithm)
    std::vector<uint8_t> text_data;
    std::string repeated_text = "COMPRESSION_TEST_DATA_REPEATED_PATTERN ";
    for (int i = 0; i < 25; i++) {
        text_data.insert(text_data.end(), repeated_text.begin(), repeated_text.end());
    }
    builder.addFile("data\\\\global\\\\test\\\\text_compression.txt", text_data);
    
    // Type 2: Binary data with patterns (good for PKWARE/LZ77)
    std::vector<uint8_t> binary_data;
    // Create a pattern of bytes that would compress well
    std::vector<uint8_t> pattern = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    for (int i = 0; i < 64; i++) {
        binary_data.insert(binary_data.end(), pattern.begin(), pattern.end());
        // Add some variation to test different compression scenarios
        if (i % 8 == 0) {
            binary_data.push_back(0xFF); // Marker byte
        }
    }
    builder.addFile("data\\\\global\\\\test\\\\binary_compression.dat", binary_data);
    
    // Type 3: Mixed content that tests multiple algorithms
    std::vector<uint8_t> mixed_data;
    // Add text portion
    std::string text_portion = "Mixed content test: ";
    mixed_data.insert(mixed_data.end(), text_portion.begin(), text_portion.end());
    // Add binary portion
    for (int i = 0; i < 100; i++) {
        mixed_data.push_back(static_cast<uint8_t>(i % 256));
    }
    // Add repeated pattern
    std::string repeat_portion = "REPEAT";
    for (int i = 0; i < 20; i++) {
        mixed_data.insert(mixed_data.end(), repeat_portion.begin(), repeat_portion.end());
    }
    builder.addFile("data\\\\global\\\\test\\\\mixed_compression.bin", mixed_data);
    
    // Type 4: Sparse data (mostly zeros - good for RLE)
    std::vector<uint8_t> sparse_data(512, 0x00); // 512 zeros
    // Add some non-zero data at specific positions
    sparse_data[100] = 0xAA;
    sparse_data[200] = 0xBB;
    sparse_data[300] = 0xCC;
    sparse_data[400] = 0xDD;
    builder.addFile("data\\\\global\\\\test\\\\sparse_compression.dat", sparse_data);
    
    // Build the mock MPQ file
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load it with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Test 1: Text compression algorithm validation
    auto text_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\text_compression.txt");
    ASSERT_FALSE(text_result.empty());
    EXPECT_EQ(text_result.size(), text_data.size());
    std::string extracted_text(text_result.begin(), text_result.end());
    std::string original_text(text_data.begin(), text_data.end());
    EXPECT_EQ(extracted_text, original_text) << "Text compression/decompression failed";
    // Verify pattern count
    size_t text_pattern_count = 0;
    size_t text_pos = 0;
    while ((text_pos = extracted_text.find("COMPRESSION_TEST_DATA", text_pos)) != std::string::npos) {
        text_pattern_count++;
        text_pos += 1;
    }
    EXPECT_EQ(text_pattern_count, 25) << "Text pattern not correctly preserved after compression";
    
    // Test 2: Binary compression algorithm validation
    auto binary_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\binary_compression.dat");
    ASSERT_FALSE(binary_result.empty());
    EXPECT_EQ(binary_result.size(), binary_data.size());
    // Verify the binary pattern is preserved
    for (size_t i = 0; i < std::min(binary_result.size(), binary_data.size()); i++) {
        EXPECT_EQ(binary_result[i], binary_data[i]) << "Binary data mismatch at position " << i;
    }
    // Count marker bytes to verify structure preservation
    size_t marker_count = std::count(binary_result.begin(), binary_result.end(), 0xFF);
    EXPECT_EQ(marker_count, 8) << "Binary structure not preserved after compression";
    
    // Test 3: Mixed content compression algorithm validation
    auto mixed_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\mixed_compression.bin");
    ASSERT_FALSE(mixed_result.empty());
    EXPECT_EQ(mixed_result.size(), mixed_data.size());
    std::string mixed_content(mixed_result.begin(), mixed_result.end());
    // Verify text portion
    EXPECT_TRUE(mixed_content.find("Mixed content test:") != std::string::npos);
    // Verify repeat portion
    size_t repeat_count = 0;
    size_t repeat_pos = 0;
    while ((repeat_pos = mixed_content.find("REPEAT", repeat_pos)) != std::string::npos) {
        repeat_count++;
        repeat_pos += 1;
    }
    EXPECT_EQ(repeat_count, 20) << "Mixed content repeat pattern not preserved";
    
    // Test 4: Sparse data compression algorithm validation
    auto sparse_result = asset_manager.loadFileData("data\\\\global\\\\test\\\\sparse_compression.dat");
    ASSERT_FALSE(sparse_result.empty());
    EXPECT_EQ(sparse_result.size(), sparse_data.size());
    // Verify sparse structure is preserved
    EXPECT_EQ(sparse_result[100], 0xAA) << "Sparse data marker 1 not preserved";
    EXPECT_EQ(sparse_result[200], 0xBB) << "Sparse data marker 2 not preserved";
    EXPECT_EQ(sparse_result[300], 0xCC) << "Sparse data marker 3 not preserved";
    EXPECT_EQ(sparse_result[400], 0xDD) << "Sparse data marker 4 not preserved";
    // Verify zero padding is preserved
    size_t zero_count = std::count(sparse_result.begin(), sparse_result.end(), 0x00);
    EXPECT_EQ(zero_count, 508) << "Sparse data zero padding not preserved";
}

// Test 7: Compression algorithm detection and selection
TEST_F(MockMPQIntegrationTest, ValidateCompressionAlgorithmDetection) {
    // Create a mock MPQ builder with different compression algorithms
    MockMPQBuilder builder;
    
    // Test data that would benefit from different algorithms
    std::vector<uint8_t> test_data;
    std::string pattern = "ALGORITHM_DETECTION_TEST ";
    for (int i = 0; i < 10; i++) {
        test_data.insert(test_data.end(), pattern.begin(), pattern.end());
    }
    
    // Add files with different compression algorithms
    builder.addFileWithCompression("data\\\\test\\\\zlib_compressed.txt", test_data, MockMPQBuilder::CompressionType::ZLIB);
    builder.addFileWithCompression("data\\\\test\\\\pkware_compressed.txt", test_data, MockMPQBuilder::CompressionType::PKWARE);
    builder.addFileWithCompression("data\\\\test\\\\bzip2_compressed.txt", test_data, MockMPQBuilder::CompressionType::BZIP2);
    builder.addFileWithCompression("data\\\\test\\\\uncompressed.txt", test_data, MockMPQBuilder::CompressionType::NONE);
    
    // Build the MPQ
    ASSERT_TRUE(builder.build(mock_mpq_path.string()));
    
    // Load with AssetManager
    ASSERT_TRUE(asset_manager.initializeWithMPQ(mock_mpq_path.string()));
    
    // Verify all files can be extracted correctly regardless of compression algorithm
    auto zlib_result = asset_manager.loadFileData("data\\\\test\\\\zlib_compressed.txt");
    auto pkware_result = asset_manager.loadFileData("data\\\\test\\\\pkware_compressed.txt");
    auto bzip2_result = asset_manager.loadFileData("data\\\\test\\\\bzip2_compressed.txt");
    auto uncompressed_result = asset_manager.loadFileData("data\\\\test\\\\uncompressed.txt");
    
    // All should succeed
    ASSERT_FALSE(zlib_result.empty());
    ASSERT_FALSE(pkware_result.empty());
    ASSERT_FALSE(bzip2_result.empty());
    ASSERT_FALSE(uncompressed_result.empty());
    
    // All should have same content
    std::string original_content(test_data.begin(), test_data.end());
    std::string zlib_content(zlib_result.begin(), zlib_result.end());
    std::string pkware_content(pkware_result.begin(), pkware_result.end());
    std::string bzip2_content(bzip2_result.begin(), bzip2_result.end());
    std::string uncompressed_content(uncompressed_result.begin(), uncompressed_result.end());
    
    EXPECT_EQ(zlib_content, original_content) << "ZLIB decompression failed";
    EXPECT_EQ(pkware_content, original_content) << "PKWARE decompression failed";
    EXPECT_EQ(bzip2_content, original_content) << "BZIP2 decompression failed";
    EXPECT_EQ(uncompressed_content, original_content) << "Uncompressed content mismatch";
    
    // Verify pattern preservation across all compression types
    for (const auto& [name, content] : std::vector<std::pair<std::string, std::string>>{
        {"ZLIB", zlib_content},
        {"PKWARE", pkware_content},
        {"BZIP2", bzip2_content},
        {"UNCOMPRESSED", uncompressed_content}
    }) {
        size_t pattern_count = 0;
        size_t pos = 0;
        while ((pos = content.find("ALGORITHM_DETECTION_TEST", pos)) != std::string::npos) {
            pattern_count++;
            pos += 1;
        }
        EXPECT_EQ(pattern_count, 10) << name << " compression did not preserve pattern correctly";
    }
    
    // Verify that the builder can report which compression algorithms were used
    auto compression_info = builder.getCompressionInfo();
    EXPECT_TRUE(compression_info.find("ZLIB") != compression_info.end()) << "ZLIB compression not recorded";
    EXPECT_TRUE(compression_info.find("PKWARE") != compression_info.end()) << "PKWARE compression not recorded";
    EXPECT_TRUE(compression_info.find("BZIP2") != compression_info.end()) << "BZIP2 compression not recorded";
    EXPECT_TRUE(compression_info.find("NONE") != compression_info.end()) << "No compression not recorded";
}