#include <iostream>
#include <fstream>
#include <vector>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;

int main() {
    // Create the multi-compressed test MPQ
    std::string test_file = "test_multi.mpq";
    std::ofstream file(test_file, std::ios::binary);
    
    // MPQ Header
    const char signature[] = {'M', 'P', 'Q', 0x1A};
    file.write(signature, 4);
    
    uint32_t header_size = 32;
    uint32_t archive_size = 2048;
    uint16_t format_version = 0;
    uint16_t block_size = 3;  // 512 * 2^3 = 4096 bytes
    uint32_t hash_table_offset = 64;
    uint32_t block_table_offset = 128;
    uint32_t hash_table_entries = 16;
    uint32_t block_table_entries = 16;
    
    file.write(reinterpret_cast<const char*>(&header_size), 4);
    file.write(reinterpret_cast<const char*>(&archive_size), 4);
    file.write(reinterpret_cast<const char*>(&format_version), 2);
    file.write(reinterpret_cast<const char*>(&block_size), 2);
    file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
    file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
    file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
    file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
    
    // Clear hash table
    file.seekp(hash_table_offset);
    for (int i = 0; i < hash_table_entries * 4; i++) {
        uint32_t empty = 0xFFFFFFFF;
        file.write(reinterpret_cast<const char*>(&empty), 4);
    }
    
    // Write one hash entry
    file.seekp(hash_table_offset);
    struct HashEntry {
        uint32_t name1;
        uint32_t name2;
        uint16_t locale;
        uint16_t platform;
        uint32_t block_index;
    };
    
    MPQLoader temp_loader;
    HashEntry test_entry;
    test_entry.name1 = temp_loader.hashString("test.txt", 1);
    test_entry.name2 = temp_loader.hashString("test.txt", 2);
    test_entry.locale = 0;
    test_entry.platform = 0;
    test_entry.block_index = 0;
    file.write(reinterpret_cast<const char*>(&test_entry), sizeof(HashEntry));
    
    // Create multi-compressed data
    std::string original_content = "Test multi-compression!";
    
    // Create PKWARE compressed data
    std::vector<uint8_t> pkware_compressed;
    pkware_compressed.push_back(0xFF); // All literals control byte
    for (char c : original_content) {
        pkware_compressed.push_back(c);
    }
    
    std::cout << "Original size: " << original_content.size() << std::endl;
    std::cout << "After PKWARE: " << pkware_compressed.size() << std::endl;
    
    // Now compress with zlib - but include compression type byte
    std::vector<uint8_t> multi_compressed;
    multi_compressed.push_back(0x0A); // ZLIB | PKWARE
    
    // Actually compress the PKWARE data with zlib
    // For simplicity, just copy it (pretend zlib didn't compress)
    multi_compressed.insert(multi_compressed.end(), 
                           pkware_compressed.begin(), 
                           pkware_compressed.end());
    
    std::cout << "Final size: " << multi_compressed.size() << std::endl;
    
    // Write block table
    file.seekp(block_table_offset);
    struct BlockEntry {
        uint32_t file_pos;
        uint32_t packed_size;
        uint32_t unpacked_size;
        uint32_t flags;
    };
    
    BlockEntry block;
    block.file_pos = 512;
    block.packed_size = multi_compressed.size();
    block.unpacked_size = original_content.size();
    block.flags = 0x80000300; // FILE_EXISTS | COMPRESS | IMPLODE
    file.write(reinterpret_cast<const char*>(&block), sizeof(BlockEntry));
    
    // Write compressed data
    file.seekp(block.file_pos);
    file.write(reinterpret_cast<const char*>(multi_compressed.data()), 
              multi_compressed.size());
    
    file.close();
    
    // Now try to extract
    MPQLoader loader;
    if (!loader.open(test_file)) {
        std::cerr << "Failed to open: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::vector<uint8_t> output;
    if (loader.extractFile("test.txt", output)) {
        std::string result(output.begin(), output.end());
        std::cout << "Extraction successful!" << std::endl;
        std::cout << "Result: " << result << std::endl;
    } else {
        std::cerr << "Extraction failed: " << loader.getLastError() << std::endl;
    }
    
    return 0;
}