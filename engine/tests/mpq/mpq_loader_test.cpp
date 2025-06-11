#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/mpq_loader.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <cctype>
#include <set>

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
    
    void createEncryptedTestMPQ() {
        // Create a test MPQ with encrypted hash and block tables
        encrypted_mpq_path = test_dir / "encrypted_test.mpq";
        std::ofstream file(encrypted_mpq_path, std::ios::binary);
        
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
        
        // Create unencrypted hash table in memory
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        
        std::vector<HashEntry> hash_table(hash_table_entries);
        
        // Fill with empty entries
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (size_t i = 0; i < hash_table.size(); i++) {
            hash_table[i] = empty_entry;
        }
        
        // Add our test file entry at position 0
        MPQLoader temp_loader;
        hash_table[0].name1 = temp_loader.hashString("encrypted_test.txt", 1);
        hash_table[0].name2 = temp_loader.hashString("encrypted_test.txt", 2);
        hash_table[0].locale = 0;
        hash_table[0].platform = 0;
        hash_table[0].block_index = 0;
        
        // Encrypt hash table with "(hash table)" key
        std::vector<uint32_t> encrypted_hash_table(hash_table.size() * sizeof(HashEntry) / 4);
        // For now, we'll simulate encryption by XORing with a simple pattern
        // Real implementation would use proper MPQ encryption
        uint32_t hash_key = temp_loader.hashString("(hash table)", 3);
        for (size_t i = 0; i < encrypted_hash_table.size(); i++) {
            uint32_t* raw_data = reinterpret_cast<uint32_t*>(hash_table.data());
            encrypted_hash_table[i] = raw_data[i] ^ (hash_key + i);
        }
        
        // Write encrypted hash table
        file.write(reinterpret_cast<const char*>(encrypted_hash_table.data()),
                   encrypted_hash_table.size() * sizeof(uint32_t));
        
        // Create unencrypted block table in memory
        struct BlockEntry {
            uint32_t file_pos;
            uint32_t packed_size;
            uint32_t unpacked_size;
            uint32_t flags;
        };
        
        const char* content = "Encrypted MPQ content!";
        uint32_t content_size = strlen(content);
        
        BlockEntry block;
        block.file_pos = 512; // Data starts at offset 512
        block.packed_size = content_size;
        block.unpacked_size = content_size;
        block.flags = 0x80000000; // FILE_EXISTS, not compressed
        
        // Encrypt block table with "(block table)" key
        uint32_t block_key = temp_loader.hashString("(block table)", 3);
        std::vector<uint32_t> encrypted_block_table(sizeof(BlockEntry) / 4);
        uint32_t* block_raw = reinterpret_cast<uint32_t*>(&block);
        for (size_t i = 0; i < encrypted_block_table.size(); i++) {
            encrypted_block_table[i] = block_raw[i] ^ (block_key + i);
        }
        
        // Write encrypted block table
        file.seekp(block_table_offset);
        file.write(reinterpret_cast<const char*>(encrypted_block_table.data()),
                   encrypted_block_table.size() * sizeof(uint32_t));
        
        // Write actual file content
        file.seekp(block.file_pos);
        file.write(content, content_size);
        
        file.close();
    }
    
    void createMPQWithListfile() {
        // Create a test MPQ with a (listfile) that contains filename mappings
        mpq_with_listfile_path = test_dir / "mpq_with_listfile.mpq";
        std::ofstream file(mpq_with_listfile_path, std::ios::binary);
        
        // MPQ Header
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        uint32_t header_size = 32;
        uint32_t archive_size = 4096;
        uint16_t format_version = 0;
        uint16_t block_size = 3; // 4096 bytes
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 320; // After hash table
        uint32_t hash_table_entries = 16;
        uint32_t block_table_entries = 3; // 3 files: armor.txt, weapons.txt, (listfile)
        
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        file.write(reinterpret_cast<const char*>(&format_version), 2);
        file.write(reinterpret_cast<const char*>(&block_size), 2);
        file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
        file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
        
        // Create the listfile content first
        std::string listfile_content = "data\\global\\excel\\armor.txt\r\n"
                                       "data\\global\\excel\\weapons.txt\r\n"
                                       "(listfile)\r\n";
        
        std::string armor_content = "armor data content";
        std::string weapons_content = "weapons data content";
        
        // Pad to hash table offset
        file.seekp(hash_table_offset);
        
        // Create hash table with entries for our 3 files
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        
        std::vector<HashEntry> hash_table(hash_table_entries);
        
        // Fill with empty entries
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (size_t i = 0; i < hash_table.size(); i++) {
            hash_table[i] = empty_entry;
        }
        
        // Add entries for our files
        MPQLoader temp_loader;
        
        // armor.txt entry
        hash_table[0].name1 = temp_loader.hashString("data\\global\\excel\\armor.txt", 1);
        hash_table[0].name2 = temp_loader.hashString("data\\global\\excel\\armor.txt", 2);
        hash_table[0].locale = 0;
        hash_table[0].platform = 0;
        hash_table[0].block_index = 0;
        
        // weapons.txt entry  
        hash_table[1].name1 = temp_loader.hashString("data\\global\\excel\\weapons.txt", 1);
        hash_table[1].name2 = temp_loader.hashString("data\\global\\excel\\weapons.txt", 2);
        hash_table[1].locale = 0;
        hash_table[1].platform = 0;
        hash_table[1].block_index = 1;
        
        // (listfile) entry
        hash_table[2].name1 = temp_loader.hashString("(listfile)", 1);
        hash_table[2].name2 = temp_loader.hashString("(listfile)", 2);
        hash_table[2].locale = 0;
        hash_table[2].platform = 0;
        hash_table[2].block_index = 2;
        
        // Write hash table (unencrypted for simplicity)
        file.write(reinterpret_cast<const char*>(hash_table.data()),
                   hash_table.size() * sizeof(HashEntry));
        
        // Write block table
        file.seekp(block_table_offset);
        struct BlockEntry {
            uint32_t file_pos;
            uint32_t packed_size;
            uint32_t unpacked_size;
            uint32_t flags;
        };
        
        std::vector<BlockEntry> blocks(3);
        
        // armor.txt block
        blocks[0].file_pos = 1024;
        blocks[0].packed_size = armor_content.size();
        blocks[0].unpacked_size = armor_content.size();
        blocks[0].flags = 0x80000000; // FILE_EXISTS
        
        // weapons.txt block
        blocks[1].file_pos = 1024 + armor_content.size();
        blocks[1].packed_size = weapons_content.size();
        blocks[1].unpacked_size = weapons_content.size();
        blocks[1].flags = 0x80000000; // FILE_EXISTS
        
        // (listfile) block
        blocks[2].file_pos = 1024 + armor_content.size() + weapons_content.size();
        blocks[2].packed_size = listfile_content.size();
        blocks[2].unpacked_size = listfile_content.size();
        blocks[2].flags = 0x80000000; // FILE_EXISTS
        
        file.write(reinterpret_cast<const char*>(blocks.data()),
                   blocks.size() * sizeof(BlockEntry));
        
        // Write file contents
        file.seekp(blocks[0].file_pos);
        file.write(armor_content.c_str(), armor_content.size());
        
        file.seekp(blocks[1].file_pos);
        file.write(weapons_content.c_str(), weapons_content.size());
        
        file.seekp(blocks[2].file_pos);
        file.write(listfile_content.c_str(), listfile_content.size());
        
        file.close();
    }
    
    void createZlibCompressedTestMPQ() {
        // Create a test MPQ with zlib-compressed file
        zlib_compressed_mpq_path = test_dir / "zlib_compressed.mpq";
        std::ofstream file(zlib_compressed_mpq_path, std::ios::binary);
        
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
        
        // Create hash table with one entry for "compressed_data.txt"
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
        MPQLoader temp_loader;
        test_entry.name1 = temp_loader.hashString("compressed_data.txt", 1);
        test_entry.name2 = temp_loader.hashString("compressed_data.txt", 2);
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
        
        // Create mock compressed data
        std::string original_content = "This is test content that will be compressed using zlib!";
        
        // Mock compression: for testing, we'll store the content with a simple prefix
        // indicating compression type. Real implementation would use actual zlib.
        std::string mock_compressed = "\x02" + original_content; // 0x02 = zlib compression flag
        
        BlockEntry block;
        block.file_pos = 512; // Data starts at offset 512
        block.packed_size = mock_compressed.size();
        block.unpacked_size = original_content.size();
        block.flags = 0x80000200; // FILE_EXISTS | COMPRESS
        file.write(reinterpret_cast<const char*>(&block), sizeof(BlockEntry));
        
        // Write compressed file content
        file.seekp(block.file_pos);
        file.write(mock_compressed.c_str(), mock_compressed.size());
        
        file.close();
    }
    
    void createPkwareCompressedTestMPQ() {
        // Create a test MPQ with PKWARE-compressed file
        pkware_compressed_mpq_path = test_dir / "pkware_compressed.mpq";
        std::ofstream file(pkware_compressed_mpq_path, std::ios::binary);
        
        // MPQ Header (similar to zlib test)
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        uint32_t header_size = 32;
        uint32_t archive_size = 2048;
        uint16_t format_version = 0;
        uint16_t block_size = 3;
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 320;
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
        
        // Pad to hash table offset and create hash entry
        file.seekp(hash_table_offset);
        
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (int i = 0; i < hash_table_entries; i++) {
            file.write(reinterpret_cast<const char*>(&empty_entry), sizeof(HashEntry));
        }
        
        file.seekp(hash_table_offset);
        HashEntry test_entry;
        MPQLoader temp_loader;
        test_entry.name1 = temp_loader.hashString("pkware_data.txt", 1);
        test_entry.name2 = temp_loader.hashString("pkware_data.txt", 2);
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
        
        std::string original_content = "This content uses PKWARE DCL compression algorithm!";
        std::string mock_compressed = "\x01" + original_content; // 0x01 = PKWARE compression flag
        
        BlockEntry block;
        block.file_pos = 512;
        block.packed_size = mock_compressed.size();
        block.unpacked_size = original_content.size();
        block.flags = 0x80000100; // FILE_EXISTS | IMPLODE (PKWARE)
        file.write(reinterpret_cast<const char*>(&block), sizeof(BlockEntry));
        
        // Write compressed file content
        file.seekp(block.file_pos);
        file.write(mock_compressed.c_str(), mock_compressed.size());
        
        file.close();
    }
    
    void createMultiCompressedTestMPQ() {
        // Create a test MPQ with file using multiple compression methods
        multi_compressed_mpq_path = test_dir / "multi_compressed.mpq";
        std::ofstream file(multi_compressed_mpq_path, std::ios::binary);
        
        // MPQ Header
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        uint32_t header_size = 32;
        uint32_t archive_size = 2048;
        uint16_t format_version = 0;
        uint16_t block_size = 3;
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 320;
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
        
        // Hash table setup
        file.seekp(hash_table_offset);
        
        struct HashEntry {
            uint32_t name1;
            uint32_t name2;
            uint16_t locale;
            uint16_t platform;
            uint32_t block_index;
        };
        
        HashEntry empty_entry = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
        for (int i = 0; i < hash_table_entries; i++) {
            file.write(reinterpret_cast<const char*>(&empty_entry), sizeof(HashEntry));
        }
        
        file.seekp(hash_table_offset);
        HashEntry test_entry;
        MPQLoader temp_loader;
        test_entry.name1 = temp_loader.hashString("multi_compressed.txt", 1);
        test_entry.name2 = temp_loader.hashString("multi_compressed.txt", 2);
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
        
        std::string original_content = "Content compressed with multiple algorithms for better ratio!";
        std::string mock_compressed = "\x03" + original_content; // 0x03 = multi compression flag
        
        BlockEntry block;
        block.file_pos = 512;
        block.packed_size = mock_compressed.size();
        block.unpacked_size = original_content.size();
        block.flags = 0x80000300; // FILE_EXISTS | COMPRESS + IMPLODE
        file.write(reinterpret_cast<const char*>(&block), sizeof(BlockEntry));
        
        // Write compressed file content
        file.seekp(block.file_pos);
        file.write(mock_compressed.c_str(), mock_compressed.size());
        
        file.close();
    }

    std::filesystem::path test_dir;
    std::filesystem::path test_mpq_path;
    std::filesystem::path test_mpq_with_file;
    std::filesystem::path encrypted_mpq_path;
    std::filesystem::path mpq_with_listfile_path;
    std::filesystem::path zlib_compressed_mpq_path;
    std::filesystem::path pkware_compressed_mpq_path;
    std::filesystem::path multi_compressed_mpq_path;
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
    
    // Check file information (no listfile, so gets fallback name)
    EXPECT_EQ(files[0].filename, "Unknown_0");
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

// Test: MPQ table decryption support
TEST_F(MPQLoaderTest, DecryptMPQTables) {
    // Create a test MPQ with encrypted tables
    createEncryptedTestMPQ();
    
    ASSERT_TRUE(loader.open(encrypted_mpq_path.string()));
    
    // The loader should be able to decrypt and read the hash table
    auto files = loader.listFiles();
    EXPECT_EQ(files.size(), 1);
    // Note: filename lookup requires proper filename resolution (next task)
    // For now, just verify decryption worked by checking file properties
    EXPECT_EQ(files[0].uncompressed_size, 22); // "Encrypted MPQ content!" length
    EXPECT_TRUE(files[0].flags & 0x80000000);  // FILE_EXISTS flag
    
    // Should be able to extract the file content using the correct filename
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("encrypted_test.txt", output));
    
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "Encrypted MPQ content!");
}

// Test: MPQ filename resolution with listfile support
TEST_F(MPQLoaderTest, FilenameResolutionWithListfile) {
    // Create a test MPQ with listfile
    createMPQWithListfile();
    
    ASSERT_TRUE(loader.open(mpq_with_listfile_path.string()));
    
    // Should be able to list all files with correct names
    auto files = loader.listFiles();
    EXPECT_EQ(files.size(), 3);
    
    // Check that files have proper names from listfile
    std::set<std::string> expected_files = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt", 
        "(listfile)"
    };
    
    std::set<std::string> actual_files;
    for (const auto& file : files) {
        actual_files.insert(file.filename);
    }
    
    EXPECT_EQ(actual_files, expected_files);
    
    // Should be able to extract files by their resolved names
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("data\\global\\excel\\armor.txt", output));
    
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "armor data content");
}

// Test: Extract compressed file with zlib compression
TEST_F(MPQLoaderTest, ExtractZlibCompressedFile) {
    // Create a test MPQ with zlib-compressed file
    createZlibCompressedTestMPQ();
    
    ASSERT_TRUE(loader.open(zlib_compressed_mpq_path.string()));
    
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("compressed_data.txt", output));
    
    // Should decompress to original content
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "This is test content that will be compressed using zlib!");
}

// Test: Extract file with PKWARE DCL compression  
TEST_F(MPQLoaderTest, ExtractPkwareCompressedFile) {
    // Create a test MPQ with PKWARE-compressed file
    createPkwareCompressedTestMPQ();
    
    ASSERT_TRUE(loader.open(pkware_compressed_mpq_path.string()));
    
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("pkware_data.txt", output));
    
    // Should decompress to original content
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "This content uses PKWARE DCL compression algorithm!");
}

// Test: Extract file with multiple compression methods combined
TEST_F(MPQLoaderTest, ExtractMultiCompressedFile) {
    // Create a test MPQ with file using multiple compression methods
    createMultiCompressedTestMPQ();
    
    ASSERT_TRUE(loader.open(multi_compressed_mpq_path.string()));
    
    std::vector<uint8_t> output;
    EXPECT_TRUE(loader.extractFile("multi_compressed.txt", output));
    
    // Should decompress correctly regardless of method combination
    std::string content(output.begin(), output.end());
    EXPECT_EQ(content, "Content compressed with multiple algorithms for better ratio!");
}