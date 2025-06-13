#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <algorithm>

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

uint32_t hashString(const std::string& str, uint32_t hash_type) {
    initCryptTable();
    
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;
    
    for (char ch : str) {
        ch = std::toupper(ch);
        seed1 = crypt_table[(hash_type * 0x100) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }
    
    return seed1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>\n";
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open: " << argv[1] << "\n";
        return 1;
    }
    
    // Read header
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    std::cout << "MPQ: " << argv[1] << "\n";
    std::cout << "Hash table entries: " << header.hash_table_entries << "\n";
    std::cout << "Block table entries: " << header.block_table_entries << "\n\n";
    
    // Read hash table
    file.seekg(header.hash_table_offset);
    std::vector<MPQHashEntry> hash_table(header.hash_table_entries);
    file.read(reinterpret_cast<char*>(hash_table.data()), 
              sizeof(MPQHashEntry) * header.hash_table_entries);
    
    // Count valid entries
    int valid_entries = 0;
    int empty_entries = 0;
    int deleted_entries = 0;
    
    for (const auto& entry : hash_table) {
        if (entry.block_index == 0xFFFFFFFF) {
            empty_entries++;
        } else if (entry.block_index == 0xFFFFFFFE) {
            deleted_entries++;
        } else if (entry.block_index < header.block_table_entries) {
            valid_entries++;
        }
    }
    
    std::cout << "Hash table analysis:\n";
    std::cout << "  Valid entries: " << valid_entries << "\n";
    std::cout << "  Empty entries: " << empty_entries << "\n";
    std::cout << "  Deleted entries: " << deleted_entries << "\n\n";
    
    // Try to find (listfile)
    uint32_t listfile_hash1 = hashString("(listfile)", 1);
    uint32_t listfile_hash2 = hashString("(listfile)", 2);
    
    std::cout << "Looking for (listfile)...\n";
    std::cout << "  Hash1: 0x" << std::hex << listfile_hash1 << "\n";
    std::cout << "  Hash2: 0x" << std::hex << listfile_hash2 << std::dec << "\n";
    
    bool found_listfile = false;
    for (size_t i = 0; i < hash_table.size(); i++) {
        const auto& entry = hash_table[i];
        if (entry.name1 == listfile_hash1 && entry.name2 == listfile_hash2) {
            std::cout << "  Found at hash index " << i << ", block " << entry.block_index << "\n";
            found_listfile = true;
        }
    }
    
    if (!found_listfile) {
        std::cout << "  Not found in hash table\n";
    }
    
    // Show first few valid block entries
    file.seekg(header.block_table_offset);
    std::vector<MPQBlockEntry> block_table(header.block_table_entries);
    file.read(reinterpret_cast<char*>(block_table.data()),
              sizeof(MPQBlockEntry) * header.block_table_entries);
    
    std::cout << "\nFirst 5 block entries:\n";
    for (size_t i = 0; i < std::min(size_t(5), block_table.size()); i++) {
        const auto& block = block_table[i];
        std::cout << "  Block " << i << ": ";
        std::cout << "pos=" << block.file_pos << ", ";
        std::cout << "packed=" << block.packed_size << ", ";
        std::cout << "unpacked=" << block.unpacked_size << ", ";
        std::cout << "flags=0x" << std::hex << block.flags << std::dec << "\n";
    }
    
    return 0;
}