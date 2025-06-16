#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <string>
#include <algorithm>

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

// Constants from mpq_constants.h
const uint32_t MPQ_FILE_ENCRYPTED = 0x00010000;
const uint32_t MPQ_FILE_ADJUSTED_KEY = 0x00020000;
const uint32_t MPQ_FILE_COMPRESS = 0x00000200;

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

void decryptFileData(std::vector<uint8_t>& data, size_t size, uint32_t key) {
    initCryptTable();
    
    // Create aligned buffer for decryption
    size_t dwords = (size + 3) / 4;
    std::vector<uint32_t> buffer(dwords);
    std::memcpy(buffer.data(), data.data(), size);
    
    uint32_t seed = 0xEEEEEEEE;
    
    for (size_t i = 0; i < dwords; ++i) {
        seed += crypt_table[0x400 + (key & 0xFF)];
        buffer[i] = buffer[i] ^ (key + seed);
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = buffer[i] + seed + (seed << 5) + 3;
    }
    
    // Copy back
    std::memcpy(data.data(), buffer.data(), size);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>" << std::endl;
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open: " << argv[1] << std::endl;
        return 1;
    }
    
    // Read header
    MPQHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
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
    
    // Find (listfile)
    uint32_t name1 = hashString("(listfile)", MPQ_HASH_NAME_A);
    uint32_t name2 = hashString("(listfile)", MPQ_HASH_NAME_B);
    
    const MPQHashEntry* hash_entry = nullptr;
    for (const auto& entry : hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            hash_entry = &entry;
            break;
        }
    }
    
    if (!hash_entry) {
        std::cerr << "(listfile) not found" << std::endl;
        return 1;
    }
    
    std::cout << "=== (listfile) Analysis ===" << std::endl;
    std::cout << "Block index: " << hash_entry->block_index << std::endl;
    
    const MPQBlockEntry& block = block_table[hash_entry->block_index];
    std::cout << "File position: " << block.file_pos << std::endl;
    std::cout << "Packed size: " << block.packed_size << std::endl;
    std::cout << "Unpacked size: " << block.unpacked_size << std::endl;
    std::cout << "Flags: 0x" << std::hex << block.flags << std::dec << std::endl;
    
    // Read raw file data
    file.seekg(block.file_pos);
    std::vector<uint8_t> raw_data(block.packed_size);
    file.read(reinterpret_cast<char*>(raw_data.data()), block.packed_size);
    
    std::cout << "\n=== Raw Data (first 64 bytes) ===" << std::endl;
    for (size_t i = 0; i < std::min(size_t(64), raw_data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                 << static_cast<int>(raw_data[i]) << " ";
        if (i % 16 == 15) std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
    
    // If encrypted, decrypt it
    if (block.flags & MPQ_FILE_ENCRYPTED) {
        std::cout << "\nFile is encrypted. Decrypting..." << std::endl;
        
        std::string filename = "(listfile)";
        uint32_t file_key = hashString(filename, MPQ_HASH_FILE_KEY);
        
        if (block.flags & MPQ_FILE_ADJUSTED_KEY) {
            file_key = (file_key + block.file_pos) ^ block.unpacked_size;
        }
        
        // Make a copy to decrypt
        std::vector<uint8_t> decrypted_data = raw_data;
        decryptFileData(decrypted_data, decrypted_data.size(), file_key);
        
        std::cout << "\n=== Decrypted Data (first 64 bytes) ===" << std::endl;
        for (size_t i = 0; i < std::min(size_t(64), decrypted_data.size()); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(decrypted_data[i]) << " ";
            if (i % 16 == 15) std::cout << std::endl;
        }
        std::cout << std::dec << std::endl;
        
        // If compressed, check compression mask
        if (block.flags & MPQ_FILE_COMPRESS) {
            std::cout << "\nFile is compressed." << std::endl;
            std::cout << "Compression mask (first byte): 0x" << std::hex 
                     << static_cast<int>(decrypted_data[0]) << std::dec << std::endl;
            
            // Decode compression mask
            uint8_t mask = decrypted_data[0];
            std::cout << "Compression types:" << std::endl;
            if (mask & 0x01) std::cout << "  - Huffman (0x01)" << std::endl;
            if (mask & 0x02) std::cout << "  - Zlib (0x02)" << std::endl;
            if (mask & 0x08) std::cout << "  - PKWARE (0x08)" << std::endl;
            if (mask & 0x10) std::cout << "  - BZip2 (0x10)" << std::endl;
            if (mask & 0x20) std::cout << "  - Sparse (0x20)" << std::endl;
            if (mask & 0x40) std::cout << "  - ADPCM Mono (0x40)" << std::endl;
            if (mask & 0x80) std::cout << "  - ADPCM Stereo (0x80)" << std::endl;
            
            // Show data after compression mask
            std::cout << "\nData after compression mask (bytes 1-32):" << std::endl;
            for (size_t i = 1; i < std::min(size_t(33), decrypted_data.size()); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << static_cast<int>(decrypted_data[i]) << " ";
                if (i % 16 == 0) std::cout << std::endl;
            }
            std::cout << std::dec << std::endl;
        }
    }
    
    return 0;
}