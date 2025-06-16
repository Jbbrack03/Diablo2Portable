#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <string>
#include <algorithm>
#include <bitset>

// MPQ structures
struct MPQHeader {
    char magic[4];        // 'MPQ\x1A'
    uint32_t header_size; // Size of this header
    uint32_t archive_size;
    uint16_t version;     // Format version  
    uint16_t block_size;  // Power of 2 (512 * 2^n)
    uint32_t hash_table_pos;
    uint32_t block_table_pos;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
};

struct MPQHashEntry {
    uint32_t name1;       // Hash of filename part 1
    uint32_t name2;       // Hash of filename part 2  
    uint16_t locale;      // Language ID
    uint16_t platform;    // Platform ID
    uint32_t block_index; // Index into block table
};

struct MPQBlockEntry {
    uint32_t file_pos;      // Position of file in archive
    uint32_t packed_size;   // Compressed size
    uint32_t unpacked_size; // Uncompressed size
    uint32_t flags;         // File flags
};

// Constants
const uint32_t MPQ_FILE_IMPLODE = 0x00000100;
const uint32_t MPQ_FILE_COMPRESS = 0x00000200;
const uint32_t MPQ_FILE_ENCRYPTED = 0x00010000;
const uint32_t MPQ_FILE_ADJUSTED_KEY = 0x00020000;
const uint32_t MPQ_FILE_SINGLE_UNIT = 0x01000000;

// Hash types
const uint32_t MPQ_HASH_NAME_A = 0x100;
const uint32_t MPQ_HASH_NAME_B = 0x200;
const uint32_t MPQ_HASH_FILE_KEY = 0x300;

// StormLib crypt table
static uint32_t crypt_table[0x500];
static bool crypt_table_initialized = false;

void initCryptTable() {
    if (crypt_table_initialized) return;
    
    uint32_t seed = 0x00100001;
    
    for (int i = 0; i < 0x100; i++) {
        for (int j = i, k = 0; k < 5; k++, j += 0x100) {
            seed = (seed * 125 + 3) % 0x2AAAAB;
            uint32_t temp = (seed & 0xFFFF) << 0x10;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            crypt_table[j] = temp | (seed & 0xFFFF);
        }
    }
    
    crypt_table_initialized = true;
}

uint32_t hashString(const std::string& str, uint32_t hash_type) {
    initCryptTable();
    
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;
    
    for (char ch : str) {
        ch = std::toupper(ch);
        seed1 = crypt_table[(hash_type * 0x100) + static_cast<uint8_t>(ch)] ^ (seed1 + seed2);
        seed2 = static_cast<uint8_t>(ch) + seed1 + seed2 + (seed2 << 5) + 3;
    }
    
    return seed1;
}

void decryptBlock(void* block, size_t size, uint32_t key) {
    initCryptTable();
    
    uint32_t* data = static_cast<uint32_t*>(block);
    size_t dword_count = size / sizeof(uint32_t);
    uint32_t seed = 0xEEEEEEEE;
    
    for (size_t i = 0; i < dword_count; ++i) {
        seed += crypt_table[0x400 + (key & 0xFF)];
        data[i] = data[i] ^ (key + seed);
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = data[i] + seed + (seed << 5) + 3;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> [filename]" << std::endl;
        return 1;
    }
    
    std::string filename = argc == 3 ? argv[2] : "(listfile)";
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open: " << argv[1] << std::endl;
        return 1;
    }
    
    // Read header
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    std::cout << "=== MPQ Header ===" << std::endl;
    std::cout << "Version: " << header.version << std::endl;
    std::cout << "Block size: " << header.block_size << " (sector size: " 
              << (512 << header.block_size) << " bytes)" << std::endl;
    
    // Read hash table
    std::vector<MPQHashEntry> hash_table(header.hash_table_entries);
    file.seekg(header.hash_table_pos);
    file.read(reinterpret_cast<char*>(hash_table.data()), 
              sizeof(MPQHashEntry) * header.hash_table_entries);
    
    // Decrypt hash table
    decryptBlock(hash_table.data(), 
                 sizeof(MPQHashEntry) * header.hash_table_entries,
                 hashString("(hash table)", MPQ_HASH_FILE_KEY));
    
    // Read block table
    std::vector<MPQBlockEntry> block_table(header.block_table_entries);
    file.seekg(header.block_table_pos);
    file.read(reinterpret_cast<char*>(block_table.data()),
              sizeof(MPQBlockEntry) * header.block_table_entries);
              
    // Decrypt block table
    decryptBlock(block_table.data(),
                 sizeof(MPQBlockEntry) * header.block_table_entries,
                 hashString("(block table)", MPQ_HASH_FILE_KEY));
    
    // Find file
    uint32_t name1 = hashString(filename, MPQ_HASH_NAME_A);
    uint32_t name2 = hashString(filename, MPQ_HASH_NAME_B);
    
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
    
    std::cout << "\n=== File: " << filename << " ===" << std::endl;
    std::cout << "Block index: " << hash_entry->block_index << std::endl;
    std::cout << "File position: " << block.file_pos << std::endl;
    std::cout << "Packed size: " << block.packed_size << std::endl;
    std::cout << "Unpacked size: " << block.unpacked_size << std::endl;
    std::cout << "Flags: 0x" << std::hex << block.flags << std::dec << std::endl;
    
    // Decode flags
    std::cout << "Flags decoded:" << std::endl;
    if (block.flags & 0x80000000) std::cout << "  - EXISTS" << std::endl;
    if (block.flags & MPQ_FILE_IMPLODE) std::cout << "  - IMPLODE (PKWARE)" << std::endl;
    if (block.flags & MPQ_FILE_COMPRESS) std::cout << "  - COMPRESS" << std::endl;
    if (block.flags & MPQ_FILE_ENCRYPTED) std::cout << "  - ENCRYPTED" << std::endl;
    if (block.flags & MPQ_FILE_ADJUSTED_KEY) std::cout << "  - ADJUSTED_KEY" << std::endl;
    if (block.flags & MPQ_FILE_SINGLE_UNIT) std::cout << "  - SINGLE_UNIT" << std::endl;
    
    // Calculate sector info
    uint32_t sector_size = 512 << header.block_size;
    bool is_single_unit = (block.flags & MPQ_FILE_SINGLE_UNIT) != 0;
    
    std::cout << "\nSector info:" << std::endl;
    std::cout << "  Sector size: " << sector_size << " bytes" << std::endl;
    std::cout << "  Single unit: " << (is_single_unit ? "YES" : "NO") << std::endl;
    
    if (!is_single_unit) {
        uint32_t num_sectors = (block.unpacked_size + sector_size - 1) / sector_size;
        std::cout << "  Number of sectors: " << num_sectors << std::endl;
        
        // For multi-sector compressed files, there's a sector offset table
        if (block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE)) {
            std::cout << "  Sector offset table size: " << (num_sectors + 1) * 4 << " bytes" << std::endl;
        }
    }
    
    // Read file data
    file.seekg(block.file_pos);
    std::vector<uint8_t> raw_data(std::min(size_t(256), size_t(block.packed_size)));
    file.read(reinterpret_cast<char*>(raw_data.data()), raw_data.size());
    
    std::cout << "\n=== Raw file data (first 256 bytes) ===" << std::endl;
    for (size_t i = 0; i < raw_data.size(); i += 16) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
        for (size_t j = 0; j < 16 && i + j < raw_data.size(); ++j) {
            std::cout << std::setw(2) << static_cast<int>(raw_data[i + j]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::dec;
    
    // If encrypted, show decrypted data
    if (block.flags & MPQ_FILE_ENCRYPTED) {
        std::cout << "\n=== Analyzing encryption ===" << std::endl;
        
        // Calculate file key
        std::string base_name = filename;
        size_t slash_pos = filename.find_last_of("\\/");
        if (slash_pos != std::string::npos) {
            base_name = filename.substr(slash_pos + 1);
        }
        
        for (char& c : base_name) {
            c = std::toupper(c);
        }
        
        uint32_t file_key = hashString(base_name, MPQ_HASH_FILE_KEY);
        std::cout << "Base encryption key: 0x" << std::hex << file_key << std::dec << std::endl;
        
        if (block.flags & MPQ_FILE_ADJUSTED_KEY) {
            file_key = (file_key + block.file_pos) ^ block.unpacked_size;
            std::cout << "Adjusted key: 0x" << std::hex << file_key << std::dec << std::endl;
        }
        
        // Decrypt first part
        std::vector<uint8_t> decrypted = raw_data;
        decryptBlock(decrypted.data(), decrypted.size() & ~3, file_key);
        
        std::cout << "\n=== Decrypted data (first 256 bytes) ===" << std::endl;
        for (size_t i = 0; i < decrypted.size(); i += 16) {
            std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
            for (size_t j = 0; j < 16 && i + j < decrypted.size(); ++j) {
                std::cout << std::setw(2) << static_cast<int>(decrypted[i + j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::dec;
        
        // If compressed and not single unit, first data is sector offset table
        if ((block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE)) && !is_single_unit) {
            std::cout << "\n=== Sector offset table ===" << std::endl;
            uint32_t num_sectors = (block.unpacked_size + sector_size - 1) / sector_size;
            
            // Show first few sector offsets
            for (size_t i = 0; i < std::min(size_t(5), size_t(num_sectors + 1)) * 4 && i < decrypted.size(); i += 4) {
                uint32_t offset = *reinterpret_cast<uint32_t*>(&decrypted[i]);
                std::cout << "  Sector " << i/4 << " offset: " << offset << std::endl;
            }
            
            // The actual compressed data starts after the sector offset table
            size_t data_start = (num_sectors + 1) * 4;
            if (data_start < decrypted.size()) {
                std::cout << "\n=== First compressed sector data ===" << std::endl;
                std::cout << "Compression mask byte: 0x" << std::hex 
                         << static_cast<int>(decrypted[data_start]) << std::dec << std::endl;
                
                uint8_t mask = decrypted[data_start];
                std::cout << "Binary: " << std::bitset<8>(mask) << std::endl;
                if (mask & 0x01) std::cout << "  - 0x01: Huffman" << std::endl;
                if (mask & 0x02) std::cout << "  - 0x02: Zlib" << std::endl;
                if (mask & 0x08) std::cout << "  - 0x08: PKWARE" << std::endl;
                if (mask & 0x10) std::cout << "  - 0x10: BZip2" << std::endl;
                if (mask & 0x20) std::cout << "  - 0x20: Sparse" << std::endl;
                if (mask & 0x40) std::cout << "  - 0x40: ADPCM Mono" << std::endl;
                if (mask & 0x80) std::cout << "  - 0x80: ADPCM Stereo" << std::endl;
            }
        } else if (block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE)) {
            // Single unit compressed file
            std::cout << "\n=== Single unit compressed data ===" << std::endl;
            std::cout << "Compression mask byte: 0x" << std::hex 
                     << static_cast<int>(decrypted[0]) << std::dec << std::endl;
            
            uint8_t mask = decrypted[0];
            std::cout << "Binary: " << std::bitset<8>(mask) << std::endl;
            if (mask & 0x01) std::cout << "  - 0x01: Huffman" << std::endl;
            if (mask & 0x02) std::cout << "  - 0x02: Zlib" << std::endl;
            if (mask & 0x08) std::cout << "  - 0x08: PKWARE" << std::endl;
            if (mask & 0x10) std::cout << "  - 0x10: BZip2" << std::endl;
            if (mask & 0x20) std::cout << "  - 0x20: Sparse" << std::endl;
            if (mask & 0x40) std::cout << "  - 0x40: ADPCM Mono" << std::endl;
            if (mask & 0x80) std::cout << "  - 0x80: ADPCM Stereo" << std::endl;
        }
    }
    
    return 0;
}