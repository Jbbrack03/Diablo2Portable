#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

// Create a simple test MPQ with known content
void createTestMPQ(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    
    // MPQ Header
    const char signature[] = {'M', 'P', 'Q', 0x1A};
    file.write(signature, 4);
    
    uint32_t header_size = 32;
    uint32_t archive_size = 4096;
    uint16_t format_version = 0;
    uint16_t block_size = 3;
    uint32_t hash_table_offset = 512;
    uint32_t block_table_offset = 1024;
    uint32_t hash_table_entries = 16;
    uint32_t block_table_entries = 2;
    
    file.write(reinterpret_cast<const char*>(&header_size), 4);
    file.write(reinterpret_cast<const char*>(&archive_size), 4);
    file.write(reinterpret_cast<const char*>(&format_version), 2);
    file.write(reinterpret_cast<const char*>(&block_size), 2);
    file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
    file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
    file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
    file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
    
    // Create hash entries
    struct HashEntry {
        uint32_t name1;
        uint32_t name2;
        uint16_t locale;
        uint16_t platform;
        uint32_t block_index;
    };
    
    MPQLoader loader;
    
    // Pad to hash table
    file.seekp(hash_table_offset);
    
    // Fill with empty entries
    HashEntry empty = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFFFFFFFF};
    for (int i = 0; i < hash_table_entries; i++) {
        file.write(reinterpret_cast<const char*>(&empty), sizeof(HashEntry));
    }
    
    // Add test file entry
    file.seekp(hash_table_offset);
    HashEntry test_entry;
    test_entry.name1 = loader.hashString("test.txt", 1);
    test_entry.name2 = loader.hashString("test.txt", 2);
    test_entry.locale = 0;
    test_entry.platform = 0;
    test_entry.block_index = 0;
    file.write(reinterpret_cast<const char*>(&test_entry), sizeof(HashEntry));
    
    // Add listfile entry
    file.seekp(hash_table_offset + sizeof(HashEntry));
    HashEntry listfile_entry;
    listfile_entry.name1 = loader.hashString("(listfile)", 1);
    listfile_entry.name2 = loader.hashString("(listfile)", 2);
    listfile_entry.locale = 0;
    listfile_entry.platform = 0;
    listfile_entry.block_index = 1;
    file.write(reinterpret_cast<const char*>(&listfile_entry), sizeof(HashEntry));
    
    // Write block table
    struct BlockEntry {
        uint32_t file_pos;
        uint32_t packed_size;
        uint32_t unpacked_size;
        uint32_t flags;
    };
    
    file.seekp(block_table_offset);
    
    // Test.txt block
    BlockEntry test_block;
    test_block.file_pos = 2048;
    test_block.packed_size = 13;
    test_block.unpacked_size = 13;
    test_block.flags = 0x80000000; // FILE_EXISTS
    file.write(reinterpret_cast<const char*>(&test_block), sizeof(BlockEntry));
    
    // Listfile block
    BlockEntry list_block;
    list_block.file_pos = 2064;
    list_block.packed_size = 9;
    list_block.unpacked_size = 9;
    list_block.flags = 0x80000000; // FILE_EXISTS
    file.write(reinterpret_cast<const char*>(&list_block), sizeof(BlockEntry));
    
    // Write file contents
    file.seekp(2048);
    file.write("Hello, World!", 13);
    
    file.seekp(2064);
    file.write("test.txt\n", 9);
    
    file.close();
    std::cout << "Created test MPQ: " << filename << "\n";
}

int main() {
    std::cout << "Creating test MPQ files...\n";
    
    createTestMPQ("test_unencrypted.mpq");
    
    // Test with our loader
    MPQLoader loader;
    if (loader.open("test_unencrypted.mpq")) {
        std::cout << "Successfully opened test MPQ\n";
        
        auto files = loader.listFiles();
        std::cout << "Files found: " << files.size() << "\n";
        for (const auto& f : files) {
            std::cout << "  - " << f.filename << "\n";
        }
        
        std::vector<uint8_t> data;
        if (loader.extractFile("test.txt", data)) {
            std::cout << "Extracted test.txt: " << std::string(data.begin(), data.end()) << "\n";
        }
    } else {
        std::cout << "Failed to open test MPQ: " << loader.getLastError() << "\n";
    }
    
    return 0;
}