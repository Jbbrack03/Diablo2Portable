#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <cstring>

struct MPQHeader {
    char signature[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
};

struct MPQHashEntry {
    uint32_t name1;
    uint32_t name2;
    uint16_t locale;
    uint16_t platform;
    uint32_t block_index;
};

struct MPQBlockEntry {
    uint32_t file_pos;
    uint32_t packed_size;
    uint32_t unpacked_size;
    uint32_t flags;
};

// Storm hash constants
static uint32_t crypt_table[0x500];
static bool crypt_initialized = false;

void initCryptTable() {
    if (crypt_initialized) return;
    
    uint32_t seed = 0x00100001;
    
    for (int index1 = 0; index1 < 0x100; index1++) {
        for (int index2 = index1, i = 0; i < 5; i++, index2 += 0x100) {
            uint32_t temp1, temp2;
            
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;
            
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);
            
            crypt_table[index2] = (temp1 | temp2);
        }
    }
    crypt_initialized = true;
}

void decryptTable(uint32_t* data, size_t length, uint32_t key) {
    uint32_t seed = 0xEEEEEEEE;
    
    for (size_t i = 0; i < length; i++) {
        seed += crypt_table[0x400 + (key & 0xFF)];
        uint32_t ch = data[i] ^ (key + seed);
        
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = ch + seed + (seed << 5) + 3;
        data[i] = ch;
    }
}

const uint32_t MPQ_KEY_HASH_TABLE = 0xC3AF3770;
const uint32_t MPQ_KEY_BLOCK_TABLE = 0xEC83B3A3;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>\n";
        return 1;
    }
    
    initCryptTable();
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open: " << argv[1] << "\n";
        return 1;
    }
    
    // Read header
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    std::cout << "Testing decryption of: " << argv[1] << "\n\n";
    
    // Read and decrypt hash table
    file.seekg(header.hash_table_offset);
    std::vector<MPQHashEntry> hash_table(header.hash_table_entries);
    file.read(reinterpret_cast<char*>(hash_table.data()), 
              sizeof(MPQHashEntry) * header.hash_table_entries);
    
    std::cout << "Hash table before decryption:\n";
    for (int i = 0; i < 3 && i < header.hash_table_entries; i++) {
        std::cout << "  Entry " << i << ": ";
        std::cout << "block=" << hash_table[i].block_index << "\n";
    }
    
    // Decrypt
    decryptTable(reinterpret_cast<uint32_t*>(hash_table.data()),
                 (sizeof(MPQHashEntry) * header.hash_table_entries) / sizeof(uint32_t),
                 MPQ_KEY_HASH_TABLE);
    
    std::cout << "\nHash table after decryption:\n";
    int valid_entries = 0;
    for (int i = 0; i < header.hash_table_entries; i++) {
        if (hash_table[i].block_index < header.block_table_entries) {
            valid_entries++;
            if (valid_entries <= 3) {
                std::cout << "  Entry " << i << ": ";
                std::cout << "block=" << hash_table[i].block_index << ", ";
                std::cout << "name1=0x" << std::hex << hash_table[i].name1 << ", ";
                std::cout << "name2=0x" << hash_table[i].name2 << std::dec << "\n";
            }
        }
    }
    std::cout << "Total valid entries: " << valid_entries << " / " << header.hash_table_entries << "\n";
    
    // Read and decrypt block table
    file.seekg(header.block_table_offset);
    std::vector<MPQBlockEntry> block_table(header.block_table_entries);
    file.read(reinterpret_cast<char*>(block_table.data()),
              sizeof(MPQBlockEntry) * header.block_table_entries);
    
    std::cout << "\nBlock table before decryption:\n";
    for (int i = 0; i < 3 && i < header.block_table_entries; i++) {
        std::cout << "  Block " << i << ": ";
        std::cout << "pos=" << block_table[i].file_pos << ", ";
        std::cout << "packed=" << block_table[i].packed_size << "\n";
    }
    
    // Decrypt
    decryptTable(reinterpret_cast<uint32_t*>(block_table.data()),
                 (sizeof(MPQBlockEntry) * header.block_table_entries) / sizeof(uint32_t),
                 MPQ_KEY_BLOCK_TABLE);
    
    std::cout << "\nBlock table after decryption:\n";
    int valid_blocks = 0;
    for (int i = 0; i < header.block_table_entries; i++) {
        if (block_table[i].file_pos > 0 && block_table[i].file_pos < header.archive_size &&
            block_table[i].unpacked_size > 0 && block_table[i].unpacked_size < 100*1024*1024) {
            valid_blocks++;
            if (valid_blocks <= 3) {
                std::cout << "  Block " << i << ": ";
                std::cout << "pos=" << block_table[i].file_pos << ", ";
                std::cout << "packed=" << block_table[i].packed_size << ", ";
                std::cout << "unpacked=" << block_table[i].unpacked_size << ", ";
                std::cout << "flags=0x" << std::hex << block_table[i].flags << std::dec << "\n";
            }
        }
    }
    std::cout << "Valid blocks: " << valid_blocks << " / " << header.block_table_entries << "\n";
    
    return 0;
}