#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <sstream>

// Minimal MPQ structures
struct MPQHeader {
    char signature[4];      // 'MPQ\x1A'
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
    uint32_t name1;         // First part of filename hash
    uint32_t name2;         // Second part of filename hash
    uint16_t locale;        // Language/locale
    uint16_t platform;      // Platform
    uint32_t block_index;   // Index into block table
};

struct MPQBlockEntry {
    uint32_t file_pos;      // File offset relative to archive start
    uint32_t packed_size;   // Compressed size
    uint32_t unpacked_size; // Uncompressed size
    uint32_t flags;         // File flags
};

// MPQ file flags
const uint32_t MPQ_FILE_EXISTS = 0x80000000;
const uint32_t MPQ_FILE_ENCRYPTED = 0x00010000;
const uint32_t MPQ_FILE_ADJUSTED_KEY = 0x00020000;
const uint32_t MPQ_FILE_SINGLE_UNIT = 0x01000000;
const uint32_t MPQ_FILE_COMPRESS = 0x00000200;

// Crypt table for StormHash
static uint32_t crypt_table[0x500];
static bool crypt_table_initialized = false;

void prepareCryptTable() {
    if (crypt_table_initialized) return;
    
    uint32_t seed = 0x00100001;
    
    for (uint32_t index1 = 0; index1 < 0x100; index1++) {
        for (uint32_t index2 = index1, i = 0; i < 5; i++, index2 += 0x100) {
            uint32_t temp1, temp2;
            
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;
            
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);
            
            crypt_table[index2] = (temp1 | temp2);
        }
    }
    
    crypt_table_initialized = true;
}

uint32_t hashString(const std::string& str, uint32_t hash_type) {
    prepareCryptTable();
    
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;
    
    for (char ch : str) {
        ch = std::toupper(ch);
        uint32_t table_index = (hash_type << 8) + static_cast<uint8_t>(ch);
        seed1 = crypt_table[table_index] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }
    
    return seed1;
}

void decryptData(uint32_t* data, size_t length, uint32_t key) {
    prepareCryptTable();
    
    uint32_t seed = 0xEEEEEEEE;
    
    for (size_t i = 0; i < length; i++) {
        seed += crypt_table[0x400 + (key & 0xFF)];
        uint32_t ch = data[i] ^ (key + seed);
        
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = ch + seed + (seed << 5) + 3;
        data[i] = ch;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> <filename>" << std::endl;
        std::cerr << "Example: " << argv[0] << " d2data.mpq \"(listfile)\"" << std::endl;
        return 1;
    }
    
    std::string mpq_path = argv[1];
    std::string filename = argv[2];
    
    std::ifstream file(mpq_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open MPQ: " << mpq_path << std::endl;
        return 1;
    }
    
    // Read header
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    // Read and decrypt hash table
    std::vector<MPQHashEntry> hash_table(header.hash_table_entries);
    file.seekg(header.hash_table_offset);
    file.read(reinterpret_cast<char*>(hash_table.data()), sizeof(MPQHashEntry) * header.hash_table_entries);
    
    // Decrypt hash table
    const uint32_t MPQ_KEY_HASH_TABLE = 0xC3AF3770;
    decryptData(reinterpret_cast<uint32_t*>(hash_table.data()),
                (sizeof(MPQHashEntry) * header.hash_table_entries) / sizeof(uint32_t),
                MPQ_KEY_HASH_TABLE);
    
    // Read and decrypt block table
    std::vector<MPQBlockEntry> block_table(header.block_table_entries);
    file.seekg(header.block_table_offset);
    file.read(reinterpret_cast<char*>(block_table.data()), sizeof(MPQBlockEntry) * header.block_table_entries);
    
    // Decrypt block table
    const uint32_t MPQ_KEY_BLOCK_TABLE = 0xEC83B3A3;
    decryptData(reinterpret_cast<uint32_t*>(block_table.data()),
                (sizeof(MPQBlockEntry) * header.block_table_entries) / sizeof(uint32_t),
                MPQ_KEY_BLOCK_TABLE);
    
    // Find the file
    uint32_t name1 = hashString(filename, 1);
    uint32_t name2 = hashString(filename, 2);
    
    const MPQHashEntry* hash_entry = nullptr;
    for (const auto& entry : hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            hash_entry = &entry;
            break;
        }
    }
    
    if (!hash_entry) {
        std::cerr << "File not found: " << filename << std::endl;
        return 1;
    }
    
    const MPQBlockEntry& block = block_table[hash_entry->block_index];
    
    std::cout << "=== File: " << filename << " ===" << std::endl;
    std::cout << "Block index: " << hash_entry->block_index << std::endl;
    std::cout << "File position: " << block.file_pos << std::endl;
    std::cout << "Packed size: " << block.packed_size << std::endl;
    std::cout << "Unpacked size: " << block.unpacked_size << std::endl;
    std::cout << "Flags: 0x" << std::hex << block.flags << std::dec << std::endl;
    
    // Decode flags
    std::cout << "Flags decoded:" << std::endl;
    if (block.flags & MPQ_FILE_EXISTS) std::cout << "  - EXISTS" << std::endl;
    if (block.flags & MPQ_FILE_COMPRESS) std::cout << "  - COMPRESS" << std::endl;
    if (block.flags & MPQ_FILE_ENCRYPTED) std::cout << "  - ENCRYPTED" << std::endl;
    if (block.flags & MPQ_FILE_ADJUSTED_KEY) std::cout << "  - ADJUSTED_KEY" << std::endl;
    if (block.flags & MPQ_FILE_SINGLE_UNIT) std::cout << "  - SINGLE_UNIT" << std::endl;
    
    // Read the file data
    file.seekg(block.file_pos);
    std::vector<uint8_t> file_data(block.packed_size);
    file.read(reinterpret_cast<char*>(file_data.data()), block.packed_size);
    
    // If encrypted, decrypt it
    if (block.flags & MPQ_FILE_ENCRYPTED) {
        // Calculate encryption key
        std::string base_name = filename;
        size_t slash_pos = filename.find_last_of("\\/");
        if (slash_pos != std::string::npos) {
            base_name = filename.substr(slash_pos + 1);
        }
        
        // Convert to uppercase for key calculation
        for (char& c : base_name) {
            c = std::toupper(c);
        }
        
        uint32_t file_key = hashString(base_name, 3);
        
        // Adjust key if needed
        if (block.flags & MPQ_FILE_ADJUSTED_KEY) {
            file_key = (file_key + block.file_pos) ^ block.unpacked_size;
        }
        
        std::cout << "\nFile encryption key: 0x" << std::hex << file_key << std::dec << std::endl;
        
        // If NOT single unit, check for sector offset table
        if (!(block.flags & MPQ_FILE_SINGLE_UNIT) && (block.flags & MPQ_FILE_COMPRESS)) {
            std::cout << "\nFile uses sector-based compression" << std::endl;
            
            // Calculate number of sectors
            const uint32_t sector_size = 4096;
            uint32_t num_sectors = (block.unpacked_size + sector_size - 1) / sector_size;
            std::cout << "Number of sectors: " << num_sectors << std::endl;
            
            // Sector offset table is at the beginning
            size_t offset_table_size = (num_sectors + 1) * sizeof(uint32_t);
            if (file_data.size() < offset_table_size) {
                std::cerr << "File too small for sector offset table!" << std::endl;
                return 1;
            }
            
            // Show raw sector offset table (before decryption)
            std::cout << "\nRaw sector offset table (encrypted):" << std::endl;
            uint32_t* raw_offsets = reinterpret_cast<uint32_t*>(file_data.data());
            for (uint32_t i = 0; i <= num_sectors && i < 10; ++i) {
                std::cout << "  [" << i << "] = " << raw_offsets[i] 
                         << " (0x" << std::hex << raw_offsets[i] << std::dec << ")" << std::endl;
            }
            if (num_sectors > 10) std::cout << "  ..." << std::endl;
            
            // According to MPQ documentation, sector offset table is encrypted with key - 1
            uint32_t sector_key = file_key - 1;
            std::cout << "\nSector offset table key: 0x" << std::hex << sector_key << std::dec << std::endl;
            
            // Make a copy to decrypt
            std::vector<uint32_t> sector_offsets(num_sectors + 1);
            memcpy(sector_offsets.data(), file_data.data(), offset_table_size);
            
            // Decrypt sector offset table
            decryptData(sector_offsets.data(), num_sectors + 1, sector_key);
            
            std::cout << "\nDecrypted sector offset table:" << std::endl;
            for (uint32_t i = 0; i <= num_sectors && i < 10; ++i) {
                std::cout << "  [" << i << "] = " << sector_offsets[i] << " bytes" << std::endl;
            }
            if (num_sectors > 10) std::cout << "  ..." << std::endl;
            
            // Validate the offsets
            bool valid = true;
            for (uint32_t i = 0; i < num_sectors; ++i) {
                if (sector_offsets[i] >= sector_offsets[i + 1] || 
                    sector_offsets[i + 1] > block.packed_size) {
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                std::cout << "\nSector offsets appear valid!" << std::endl;
                
                // Show sector sizes
                std::cout << "\nSector sizes:" << std::endl;
                for (uint32_t i = 0; i < num_sectors && i < 5; ++i) {
                    uint32_t size = sector_offsets[i + 1] - sector_offsets[i];
                    std::cout << "  Sector " << i << ": " << size << " bytes" << std::endl;
                }
                if (num_sectors > 5) std::cout << "  ..." << std::endl;
            } else {
                std::cout << "\nSector offsets invalid! Encryption key might be wrong." << std::endl;
            }
        }
    }
    
    return 0;
}