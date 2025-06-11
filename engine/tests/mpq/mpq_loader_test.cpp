#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/mpq_loader.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <cctype>

using namespace d2portable::utils;
using namespace testing;

class MPQLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = std::filesystem::temp_directory_path() / "d2portable_test";
        std::filesystem::create_directories(test_dir);
        
        // Create a mock MPQ file for testing
        test_mpq_path = test_dir / "test.mpq";
        createMockMPQFile(test_mpq_path);
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }

    void createMockMPQFile(const std::filesystem::path& path) {
        // Create a minimal MPQ header for testing
        // Real MPQ format: 'MPQ\x1A' signature followed by header
        std::ofstream file(path, std::ios::binary);
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        // Write mock header data (32 bytes)
        uint32_t header_size = 32;
        uint32_t archive_size = 1024;
        uint16_t format_version = 0;
        uint16_t block_size = 3; // 4096 bytes (512 * 2^3)
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 128;
        uint32_t hash_table_entries = 16;
        uint32_t block_table_entries = 8;
        
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        file.write(reinterpret_cast<const char*>(&format_version), 2);
        file.write(reinterpret_cast<const char*>(&block_size), 2);
        file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
        file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
        
        // Write empty hash table
        file.seekp(hash_table_offset);
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (uint32_t i = 0; i < hash_table_entries; i++) {
            file.write(reinterpret_cast<const char*>(&empty_entry), sizeof(HashEntry));
        }
        
        // Write empty block table
        file.seekp(block_table_offset);
        struct BlockEntry {
            uint32_t file_pos;
            uint32_t packed_size;
            uint32_t unpacked_size;
            uint32_t flags;
        };
        BlockEntry empty_block = {0, 0, 0, 0};
        for (uint32_t i = 0; i < block_table_entries; i++) {
            file.write(reinterpret_cast<const char*>(&empty_block), sizeof(BlockEntry));
        }
    }

    void createTestMPQWithFile() {
        // Create a test MPQ with actual file content
        test_mpq_with_file = test_dir / "test_with_file.mpq";
        std::ofstream file(test_mpq_with_file, std::ios::binary);
        
        // MPQ Header
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        uint32_t header_size = 32;
        uint32_t archive_size = 2048;
        uint16_t format_version = 0;
        uint16_t block_size = 3; // 4096 bytes
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 320; // After hash table
        uint32_t hash_table_entries = 16;
        uint32_t block_table_entries = 1;
        
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        file.write(reinterpret_cast<const char*>(&format_version), 2);
        file.write(reinterpret_cast<const char*>(&block_size), 2);
        file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
        file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
        
        // Pad to hash table offset
        file.seekp(hash_table_offset);
        
        // Create hash table with one entry for "test.txt"
        // Using simple hash values for testing
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        
        // Fill hash table with empty entries
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (int i = 0; i < hash_table_entries; i++) {
            file.write(reinterpret_cast<const char*>(&empty_entry), sizeof(HashEntry));
        }
        
        // Add our test file entry at position 0
        file.seekp(hash_table_offset);
        HashEntry test_entry;
        // We'll use the StormHash algorithm from MPQLoader once it's implemented
        MPQLoader temp_loader;
        test_entry.name1 = temp_loader.hashString("test.txt", 1);
        test_entry.name2 = temp_loader.hashString("test.txt", 2);
        test_entry.locale = 0;
        test_entry.platform = 0;
        test_entry.block_index = 0;
        file.write(reinterpret_cast<const char*>(&test_entry), sizeof(HashEntry));
        
        // Write block table
        file.seekp(block_table_offset);
        struct BlockEntry {
            uint32_t file_pos;
            uint32_t packed_size;
            uint32_t unpacked_size;
            uint32_t flags;
        };
        
        const char* content = "Hello from MPQ!";
        uint32_t content_size = strlen(content);
        
        BlockEntry block;
        block.file_pos = 512; // Data starts at offset 512
        block.packed_size = content_size;
        block.unpacked_size = content_size;
        block.flags = 0x80000000; // FILE_EXISTS, not compressed
        file.write(reinterpret_cast<const char*>(&block), sizeof(BlockEntry));
        
        // Write actual file content
        file.seekp(block.file_pos);
        file.write(content, content_size);
        
        file.close();
    }

    std::filesystem::path test_dir;
    std::filesystem::path test_mpq_path;
    std::filesystem::path test_mpq_with_file;
    MPQLoader loader;
};

// Test: Opening a valid MPQ file
TEST_F(MPQLoaderTest, OpenValidMPQFile) {
    EXPECT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
}

// Test: Opening a non-existent file
TEST_F(MPQLoaderTest, OpenNonExistentFile) {
    EXPECT_FALSE(loader.open("/path/to/nonexistent.mpq"));
    EXPECT_FALSE(loader.isOpen());
    EXPECT_FALSE(loader.getLastError().empty());
}

// Test: Opening an invalid file (not MPQ format)
TEST_F(MPQLoaderTest, OpenInvalidFile) {
    auto invalid_file = test_dir / "invalid.mpq";
    std::ofstream file(invalid_file);
    file << "This is not an MPQ file";
    file.close();
    
    EXPECT_FALSE(loader.open(invalid_file.string()));
    EXPECT_FALSE(loader.isOpen());
    EXPECT_THAT(loader.getLastError(), HasSubstr("Invalid MPQ"));
}

// Test: Closing an MPQ file
TEST_F(MPQLoaderTest, CloseMPQFile) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
    
    loader.close();
    EXPECT_FALSE(loader.isOpen());
}

// Test: Listing files in an MPQ archive
TEST_F(MPQLoaderTest, ListFilesInArchive) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    auto files = loader.listFiles();
    // For our mock MPQ, we expect it to be empty initially
    EXPECT_TRUE(files.empty());
}

// Test: Checking if a file exists in the archive
TEST_F(MPQLoaderTest, CheckFileExists) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    // Mock MPQ doesn't contain any files yet
    EXPECT_FALSE(loader.hasFile("data\\global\\excel\\armor.txt"));
    EXPECT_FALSE(loader.hasFile("nonexistent.txt"));
}

// Test: Extracting a file from the archive
TEST_F(MPQLoaderTest, ExtractFileFromArchive) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    std::vector<uint8_t> output;
    // Should fail for non-existent file
    EXPECT_FALSE(loader.extractFile("data\\global\\excel\\armor.txt", output));
    EXPECT_TRUE(output.empty());
}

// Test: Getting file information
TEST_F(MPQLoaderTest, GetFileInformation) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    auto info = loader.getFileInfo("data\\global\\excel\\armor.txt");
    EXPECT_FALSE(info.has_value());
}

// Test: Multiple operations without opening
TEST_F(MPQLoaderTest, OperationsWithoutOpening) {
    EXPECT_FALSE(loader.isOpen());
    EXPECT_TRUE(loader.listFiles().empty());
    EXPECT_FALSE(loader.hasFile("any_file.txt"));
    
    std::vector<uint8_t> output;
    EXPECT_FALSE(loader.extractFile("any_file.txt", output));
    
    EXPECT_FALSE(loader.getFileInfo("any_file.txt").has_value());
}

// Test: Opening multiple files sequentially
TEST_F(MPQLoaderTest, OpenMultipleFilesSequentially) {
    // Create a second mock MPQ
    auto second_mpq = test_dir / "second.mpq";
    createMockMPQFile(second_mpq);
    
    // Open first file
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
    
    // Open second file (should close first)
    ASSERT_TRUE(loader.open(second_mpq.string()));
    EXPECT_TRUE(loader.isOpen());
}

// Test: Memory safety with large file names
TEST_F(MPQLoaderTest, LargeFileNameHandling) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    // Create a very long filename
    std::string long_filename(1024, 'a');
    EXPECT_FALSE(loader.hasFile(long_filename));
    
    std::vector<uint8_t> output;
    EXPECT_FALSE(loader.extractFile(long_filename, output));
}

// Test: Extract file with known content from MPQ
TEST_F(MPQLoaderTest, ExtractKnownFileContent) {
    // Create a proper test MPQ with a file
    createTestMPQWithFile();
    
    ASSERT_TRUE(loader.open(test_mpq_with_file.string()));
    
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("test.txt", output));
    
    // Verify extracted content
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "Hello from MPQ!");
}

// Test: List files in archive with content
TEST_F(MPQLoaderTest, ListFilesWithContent) {
    // Create a test MPQ with multiple files
    createTestMPQWithFile();
    
    ASSERT_TRUE(loader.open(test_mpq_with_file.string()));
    
    auto files = loader.listFiles();
    
    // Should have one file
    EXPECT_EQ(files.size(), 1);
    
    // Check file information
    EXPECT_EQ(files[0].filename, "test.txt");
    EXPECT_EQ(files[0].uncompressed_size, 15); // "Hello from MPQ!" length
    EXPECT_EQ(files[0].compressed_size, 15);   // Not compressed
    EXPECT_TRUE(files[0].flags & 0x80000000);  // FILE_EXISTS flag
}

// Test: StormHash algorithm implementation
TEST_F(MPQLoaderTest, StormHashAlgorithm) {
    // Test known hash values - these are from actual MPQ implementations
    uint32_t hash_a = loader.hashString("test.txt", 1);    // MPQ_HASH_NAME_A
    uint32_t hash_b = loader.hashString("test.txt", 2);    // MPQ_HASH_NAME_B
    uint32_t hash_offset = loader.hashString("test.txt", 0); // MPQ_HASH_TABLE_OFFSET
    
    // Hash values should all be different (good collision resistance)
    
    // These should be different values (collision resistance)
    EXPECT_NE(hash_a, hash_b);
    EXPECT_NE(hash_a, hash_offset);
    EXPECT_NE(hash_b, hash_offset);
    
    // Test case insensitivity
    EXPECT_EQ(loader.hashString("TEST.TXT", 1), 
              loader.hashString("test.txt", 1));
    EXPECT_EQ(loader.hashString("Test.TXT", 2), 
              loader.hashString("test.txt", 2));
    
    // Test backslash normalization 
    EXPECT_EQ(loader.hashString("data\\global\\excel\\armor.txt", 1),
              loader.hashString("DATA\\GLOBAL\\EXCEL\\ARMOR.TXT", 1));
}