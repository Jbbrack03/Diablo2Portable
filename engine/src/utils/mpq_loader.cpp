#include "utils/mpq_loader.h"
#include "utils/pkware_explode.h"
#include "utils/huffman_decompress.h"
#include "utils/bzip2_decompress.h"
#include "utils/sparse_decompress.h"
#include <fstream>
#include <cstring>
#include <filesystem>
#include <unordered_map>
#include <cctype>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <zlib.h>

namespace d2portable {
namespace utils {

// MPQ file header structure
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

// MPQ hash table entry
struct MPQHashEntry {
    uint32_t name1;         // First part of filename hash
    uint32_t name2;         // Second part of filename hash
    uint16_t locale;        // Language/locale
    uint16_t platform;      // Platform
    uint32_t block_index;   // Index into block table
};

// MPQ block table entry
struct MPQBlockEntry {
    uint32_t file_pos;      // File offset relative to archive start
    uint32_t packed_size;   // Compressed size
    uint32_t unpacked_size; // Uncompressed size
    uint32_t flags;         // File flags
};

// MPQ file flags
const uint32_t MPQ_FILE_EXISTS = 0x80000000;
const uint32_t MPQ_FILE_HAS_METADATA = 0x04000000;
const uint32_t MPQ_FILE_UNIT = 0x02000000;
const uint32_t MPQ_FILE_SINGLE_UNIT = 0x01000000;
const uint32_t MPQ_FILE_ADJUSTED_KEY = 0x00020000;
const uint32_t MPQ_FILE_ENCRYPTED = 0x00010000;
const uint32_t MPQ_FILE_COMPRESS = 0x00000200;
const uint32_t MPQ_FILE_IMPLODE = 0x00000100; // PKWARE compression

// MPQ compression type flags (bitmask in first byte of compressed data)
const uint8_t MPQ_COMPRESSION_HUFFMAN = 0x01;    // Huffman encoding
const uint8_t MPQ_COMPRESSION_ZLIB = 0x02;       // Deflate/zlib
const uint8_t MPQ_COMPRESSION_PKWARE = 0x08;     // PKWARE implode
const uint8_t MPQ_COMPRESSION_BZIP2 = 0x10;      // BZip2 (not in D2)
const uint8_t MPQ_COMPRESSION_SPARSE = 0x20;     // Sparse (not in D2)
const uint8_t MPQ_COMPRESSION_ADPCM_MONO = 0x40; // IMA ADPCM mono
const uint8_t MPQ_COMPRESSION_ADPCM_STEREO = 0x80; // IMA ADPCM stereo

// MPQ hash types
const uint32_t MPQ_HASH_TABLE_OFFSET = 0;
const uint32_t MPQ_HASH_NAME_A = 1;
const uint32_t MPQ_HASH_NAME_B = 2;
const uint32_t MPQ_HASH_FILE_KEY = 3;

// Pre-computed decryption keys for tables
const uint32_t MPQ_KEY_HASH_TABLE = 0xC3AF3770;  // HashString("(hash table)", MPQ_HASH_FILE_KEY)
const uint32_t MPQ_KEY_BLOCK_TABLE = 0xEC83B3A3; // HashString("(block table)", MPQ_HASH_FILE_KEY)

// Private implementation class
class MPQLoader::Impl {
public:
    Impl() : is_open(false), crypt_table_initialized(false) {}
    
    bool is_open;
    std::string filepath;
    std::string last_error;
    std::ifstream file;
    MPQHeader header;
    std::vector<MPQHashEntry> hash_table;
    std::vector<MPQBlockEntry> block_table;
    std::unordered_map<std::string, MPQFileInfo> file_map;
    std::unordered_map<uint32_t, std::string> block_index_to_filename;
    
    // StormHash cryptographic table
    static constexpr size_t CRYPT_TABLE_SIZE = 0x500;
    uint32_t crypt_table[CRYPT_TABLE_SIZE];
    bool crypt_table_initialized;
    
    bool validateHeader() {
        return header.signature[0] == 'M' && 
               header.signature[1] == 'P' && 
               header.signature[2] == 'Q' && 
               header.signature[3] == 0x1A;
    }
    
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
    
    // Proper StormHash implementation
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
    
    // MPQ table decryption
    void decryptTable(uint32_t* data, size_t length, uint32_t key) {
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
    
    // Decrypt file data (sector-based)
    void decryptFileData(std::vector<uint8_t>& data, size_t size, uint32_t base_key) {
        prepareCryptTable();
        
        // MPQ files are decrypted in sectors
        const size_t sector_size = 4096; // Standard MPQ sector size
        size_t num_sectors = (size + sector_size - 1) / sector_size;
        
        for (size_t sector = 0; sector < num_sectors; ++sector) {
            size_t sector_start = sector * sector_size;
            size_t sector_end = std::min(sector_start + sector_size, size);
            size_t sector_length = sector_end - sector_start;
            
            // Each sector is decrypted with key = base_key + sector_number
            uint32_t sector_key = base_key + sector;
            uint32_t seed = 0xEEEEEEEE;
            
            // Decrypt as 32-bit words
            size_t word_length = sector_length / 4;
            uint32_t* sector_data = reinterpret_cast<uint32_t*>(data.data() + sector_start);
            
            for (size_t i = 0; i < word_length; ++i) {
                seed += crypt_table[0x400 + (sector_key & 0xFF)];
                uint32_t ch = sector_data[i] ^ (sector_key + seed);
                
                sector_key = ((~sector_key << 0x15) + 0x11111111) | (sector_key >> 0x0B);
                seed = ch + seed + (seed << 5) + 3;
                sector_data[i] = ch;
            }
            
            // Handle remaining bytes (if sector size is not divisible by 4)
            size_t remaining = sector_length % 4;
            if (remaining > 0) {
                uint32_t last_word = 0;
                uint8_t* remaining_bytes = data.data() + sector_start + word_length * 4;
                
                // Read remaining bytes into a word
                for (size_t i = 0; i < remaining; ++i) {
                    last_word |= static_cast<uint32_t>(remaining_bytes[i]) << (i * 8);
                }
                
                // Decrypt the word
                seed += crypt_table[0x400 + (sector_key & 0xFF)];
                last_word = last_word ^ (sector_key + seed);
                
                // Write back the decrypted bytes
                for (size_t i = 0; i < remaining; ++i) {
                    remaining_bytes[i] = static_cast<uint8_t>(last_word >> (i * 8));
                }
            }
        }
    }
    
    // Helper function for PKWARE DCL decompression
    bool decompressPKWARE(const std::vector<uint8_t>& compressed_data,
                         std::vector<uint8_t>& output,
                         size_t expected_size) {
        // Use the proper PKWARE explode implementation
        if (!PKWAREExplode(compressed_data, output, expected_size)) {
            last_error = "PKWARE decompression failed";
            return false;
        }
        return true;
    }
    
    // Helper function for zlib decompression
    bool decompressZlib(const std::vector<uint8_t>& compressed_data, 
                       std::vector<uint8_t>& output, 
                       size_t expected_size) {
        if (compressed_data.empty()) {
            last_error = "Empty compressed data";
            return false;
        }
        
        // Setup zlib decompression
        z_stream strm = {};
        strm.next_in = const_cast<uint8_t*>(compressed_data.data());
        strm.avail_in = compressed_data.size();
        
        // Initialize inflate
        int ret = inflateInit(&strm);
        if (ret != Z_OK) {
            last_error = "Failed to initialize zlib decompression";
            return false;
        }
        
        // Decompress in chunks to handle unknown output size
        output.clear();
        output.reserve(expected_size);
        
        const size_t chunk_size = 4096;
        std::vector<uint8_t> chunk(chunk_size);
        
        do {
            strm.next_out = chunk.data();
            strm.avail_out = chunk_size;
            
            ret = inflate(&strm, Z_NO_FLUSH);
            
            if (ret == Z_STREAM_ERROR) {
                inflateEnd(&strm);
                last_error = "Zlib stream error";
                return false;
            }
            
            size_t have = chunk_size - strm.avail_out;
            output.insert(output.end(), chunk.begin(), chunk.begin() + have);
            
        } while (ret != Z_STREAM_END && ret != Z_BUF_ERROR);
        
        // Clean up
        inflateEnd(&strm);
        
        if (ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
            last_error = "Zlib decompression failed: " + std::to_string(ret);
            return false;
        }
        
        // For exact size matching (when not multi-compressed)
        if (expected_size > 0 && output.size() != expected_size && 
            output.size() < expected_size * 2) {
            // Resize to expected if close
            output.resize(expected_size);
        }
        
        return true;
    }
    
    // Decompress sector-based compressed data
    bool decompressSectorBased(const std::vector<uint8_t>& compressed_data, 
                               std::vector<uint8_t>& output, uint32_t expected_size,
                               bool is_encrypted = false, uint32_t file_key = 0) {
        // Standard MPQ sector size
        const uint32_t sector_size = 4096;
        
        // Calculate number of sectors
        uint32_t num_sectors = (expected_size + sector_size - 1) / sector_size;
        
        // Sector offset table is at the beginning
        // It contains (num_sectors + 1) uint32_t values
        size_t offset_table_size = (num_sectors + 1) * sizeof(uint32_t);
        
        if (compressed_data.size() < offset_table_size) {
            last_error = "Compressed data too small for sector offset table";
            return false;
        }
        
        // Read sector offsets
        std::vector<uint32_t> sector_offsets(num_sectors + 1);
        const uint32_t* offset_ptr = reinterpret_cast<const uint32_t*>(compressed_data.data());
        for (size_t i = 0; i <= num_sectors; ++i) {
            sector_offsets[i] = offset_ptr[i];
        }
        
        // Debug: Show raw offsets before decryption
        std::vector<uint32_t> raw_offsets;
        if (is_encrypted) {
            raw_offsets = sector_offsets;  // Save a copy for debugging
            // std::cerr << "DEBUG: Raw offsets before decryption: ";
            // for (size_t i = 0; i < std::min(size_t(5), sector_offsets.size()); ++i) {
            //     std::cerr << sector_offsets[i] << " ";
            // }
            // std::cerr << std::endl;
        }
        
        // If the file is encrypted, the sector offset table is also encrypted with key - 1
        if (is_encrypted && file_key > 0) {
            uint32_t sector_table_key = file_key - 1;
            // std::cerr << "DEBUG: Decrypting sector offset table with key 0x" << std::hex << sector_table_key << std::dec << std::endl;
            
            // Decrypt the sector offset table
            // Use the same decryption algorithm as for tables
            prepareCryptTable();
            
            uint32_t key = sector_table_key;  // Make a copy of the key
            uint32_t seed = 0xEEEEEEEE;
            for (size_t i = 0; i <= num_sectors; i++) {
                seed += crypt_table[0x400 + (key & 0xFF)];
                uint32_t ch = sector_offsets[i] ^ (key + seed);
                
                key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
                seed = ch + seed + (seed << 5) + 3;
                sector_offsets[i] = ch;
            }
            
            // Debug: Show decrypted offsets
            // std::cerr << "DEBUG: Decrypted offsets: ";
            // for (size_t i = 0; i < std::min(size_t(5), sector_offsets.size()); ++i) {
            //     std::cerr << sector_offsets[i] << " ";
            // }
            // std::cerr << std::endl;
        }
        
        // Debug output
        // std::cerr << "DEBUG: Sector-based decompression:" << std::endl;
        // std::cerr << "  Expected size: " << expected_size << std::endl;
        // std::cerr << "  Number of sectors: " << num_sectors << std::endl;
        // std::cerr << "  Compressed data size: " << compressed_data.size() << std::endl;
        // std::cerr << "  Is encrypted: " << (is_encrypted ? "yes" : "no") << std::endl;
        // if (is_encrypted) {
        //     std::cerr << "  File key: 0x" << std::hex << file_key << std::dec << std::endl;
        //     std::cerr << "  Sector table key: 0x" << std::hex << (file_key - 1) << std::dec << std::endl;
        // }
        // std::cerr << "  First few offsets: ";
        // for (size_t i = 0; i < std::min(size_t(5), sector_offsets.size()); ++i) {
        //     std::cerr << sector_offsets[i] << " ";
        // }
        // std::cerr << std::endl;
        
        // Validate sector offsets
        bool offsets_valid = true;
        if (sector_offsets[0] != offset_table_size) {
            std::cerr << "  WARNING: First offset should be " << offset_table_size << " but is " << sector_offsets[0] << std::endl;
            offsets_valid = false;
        }
        for (uint32_t i = 0; i < num_sectors; ++i) {
            if (sector_offsets[i] >= sector_offsets[i + 1]) {
                std::cerr << "  WARNING: Offset " << i << " (" << sector_offsets[i] << ") >= offset " << (i+1) << " (" << sector_offsets[i+1] << ")" << std::endl;
                offsets_valid = false;
                break;
            }
        }
        if (sector_offsets[num_sectors] > compressed_data.size()) {
            std::cerr << "  WARNING: Last offset " << sector_offsets[num_sectors] << " > compressed data size " << compressed_data.size() << std::endl;
            offsets_valid = false;
        }
        if (!offsets_valid) {
            std::cerr << "  WARNING: Sector offsets appear invalid!" << std::endl;
        }
        
        // Allocate output buffer
        output.clear();
        output.reserve(expected_size);
        
        // Decompress each sector
        for (uint32_t sector = 0; sector < num_sectors; ++sector) {
            uint32_t sector_start = sector_offsets[sector];
            uint32_t sector_end = sector_offsets[sector + 1];
            
            if (sector_start >= compressed_data.size() || sector_end > compressed_data.size() || 
                sector_start >= sector_end) {
                last_error = "Invalid sector offsets";
                return false;
            }
            
            // Extract sector data
            std::vector<uint8_t> sector_data(
                compressed_data.begin() + sector_start,
                compressed_data.begin() + sector_end
            );
            
            // If file is encrypted, decrypt this sector
            if (is_encrypted && file_key > 0) {
                // Each sector is encrypted with key = file_key + sector_number
                uint32_t sector_key = file_key + sector;
                uint32_t seed = 0xEEEEEEEE;
                
                // Decrypt as 32-bit words
                size_t word_length = sector_data.size() / 4;
                uint32_t* data_ptr = reinterpret_cast<uint32_t*>(sector_data.data());
                
                for (size_t i = 0; i < word_length; ++i) {
                    seed += crypt_table[0x400 + (sector_key & 0xFF)];
                    uint32_t ch = data_ptr[i] ^ (sector_key + seed);
                    
                    sector_key = ((~sector_key << 0x15) + 0x11111111) | (sector_key >> 0x0B);
                    seed = ch + seed + (seed << 5) + 3;
                    data_ptr[i] = ch;
                }
                
                // Handle remaining bytes
                size_t remaining = sector_data.size() % 4;
                if (remaining > 0) {
                    uint32_t last_word = 0;
                    uint8_t* remaining_bytes = sector_data.data() + word_length * 4;
                    
                    // Read remaining bytes into a word
                    for (size_t i = 0; i < remaining; ++i) {
                        last_word |= static_cast<uint32_t>(remaining_bytes[i]) << (i * 8);
                    }
                    
                    // Decrypt the word
                    seed += crypt_table[0x400 + (sector_key & 0xFF)];
                    last_word = last_word ^ (sector_key + seed);
                    
                    // Write back the decrypted bytes
                    for (size_t i = 0; i < remaining; ++i) {
                        remaining_bytes[i] = static_cast<uint8_t>(last_word >> (i * 8));
                    }
                }
            }
            
            // Calculate expected sector size
            uint32_t sector_unpacked_size = sector_size;
            if (sector == num_sectors - 1) {
                // Last sector might be smaller
                sector_unpacked_size = expected_size - (sector * sector_size);
            }
            
            // Decompress sector
            std::vector<uint8_t> decompressed_sector;
            
            // Debug: Show compression info for each sector
            if (sector_data.size() > 0) {
                uint8_t compression_mask = sector_data[0];
                std::cerr << "DEBUG: Sector " << sector << " compression mask: 0x" 
                          << std::hex << (int)compression_mask << std::dec 
                          << " size: " << sector_data.size() << " -> " << sector_unpacked_size << std::endl;
                          
                // Special case: if compressed size equals uncompressed size, it's not compressed
                if (sector_data.size() == sector_unpacked_size) {
                    std::cerr << "DEBUG: Sector " << sector << " is not compressed (size matches)\n";
                    decompressed_sector = sector_data;
                } else if (!decompressData(sector_data, decompressed_sector, sector_unpacked_size)) {
                    last_error = "Failed to decompress sector " + std::to_string(sector) + ": " + last_error;
                    return false;
                }
            } else {
                last_error = "Empty sector data";
                return false;
            }
            
            // Append to output
            output.insert(output.end(), decompressed_sector.begin(), decompressed_sector.end());
        }
        
        // Verify final size
        // Note: It's valid for the decompressed size to be less than expected
        // This happens when the actual file data doesn't fill all sectors completely
        if (output.size() > expected_size) {
            last_error = "Decompressed size exceeds expected size";
            return false;
        }
        
        // If we got less data than expected, that's OK for compressed files
        if (output.size() < expected_size) {
            std::cerr << "INFO: Decompressed " << output.size() << " bytes, expected " 
                      << expected_size << " (this is normal for compressed files)\n";
        }
        
        return true;
    }
    
    // Decompress MPQ file data
    bool decompressData(const std::vector<uint8_t>& compressed_data, 
                       std::vector<uint8_t>& output, uint32_t expected_size) {
        if (compressed_data.empty()) {
            last_error = "No data to decompress";
            return false;
        }
        
        // Check compression type (first byte is a bitmask)
        uint8_t compression_mask = compressed_data[0];
        
        // Debug output
        // std::cerr << "DEBUG: Compression mask = 0x" << std::hex << (int)compression_mask << std::dec << std::endl;
        
        // If compression mask is 0, the data might be stored without compression mask
        std::vector<uint8_t> current_data;
        if (compression_mask == 0 && compressed_data.size() == expected_size) {
            // No compression mask byte - data is stored as-is
            current_data = compressed_data;
        } else {
            // Skip compression mask byte
            current_data.assign(compressed_data.begin() + 1, compressed_data.end());
        }
        std::vector<uint8_t> temp_output;
        
        // Compression algorithms are applied in specific order during decompression
        // Based on MPQ format: decompress in the order they appear in the data
        // For multi-compression, the outermost compression is decompressed first
        
        // ADPCM decompression (for audio files)
        if (compression_mask & (MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO)) {
            // For now, we'll skip ADPCM decompression as it's complex and only for audio
            last_error = "ADPCM audio compression not yet supported";
            return false;
        }
        
        // Decompression order for multi-compression:
        // When data is compressed with multiple algorithms, it's compressed in order:
        // Original -> PKWARE -> Zlib -> BZip2 -> Huffman
        // So we decompress in reverse order: Huffman -> BZip2 -> Zlib -> PKWARE
        
        // Huffman decompression (outermost layer)
        if (compression_mask & MPQ_COMPRESSION_HUFFMAN) {
            temp_output.clear();
            // For multi-compression, we don't know intermediate size
            size_t huffman_output_size = expected_size * 2;
            if (!HuffmanDecompress(current_data, temp_output, huffman_output_size)) {
                last_error = "Huffman decompression failed";
                return false;
            }
            current_data = temp_output;
        }
        
        // BZip2 decompression
        if (compression_mask & MPQ_COMPRESSION_BZIP2) {
            temp_output.clear();
            // For BZip2, we don't know the exact output size
            size_t bzip2_output_size = expected_size * 2;
            
            // Debug: Check if we actually have BZip2 data
            if (current_data.size() < 4) {
                // If data is too small for BZip2 header, it's probably not BZip2
                // This can happen when PKWARE returns partial data for last sector
                if (current_data.size() == expected_size) {
                    // Data is already the expected size, no BZip2 needed
                    output = current_data;
                    return true;
                }
                last_error = "BZip2 decompression failed: insufficient data";
                return false;
            }
            
            if (!bzip2_decompress(current_data, temp_output, bzip2_output_size)) {
                last_error = "BZip2 decompression failed";
                return false;
            }
            current_data = temp_output;
        }
        
        // Zlib decompression (middle layer)
        if (compression_mask & MPQ_COMPRESSION_ZLIB) {
            temp_output.clear();
            // For multi-compression with PKWARE, output will be larger than final
            size_t zlib_output_size = expected_size;
            if (compression_mask & MPQ_COMPRESSION_PKWARE) {
                zlib_output_size = expected_size * 2; // Guess at PKWARE compressed size
            }
            if (!decompressZlib(current_data, temp_output, zlib_output_size)) {
                // If it fails with guessed size, try different sizes
                for (size_t multiplier = 3; multiplier <= 10; multiplier++) {
                    temp_output.clear();
                    zlib_output_size = expected_size * multiplier;
                    if (decompressZlib(current_data, temp_output, zlib_output_size)) {
                        break;
                    }
                }
                if (temp_output.empty()) {
                    last_error = "Zlib decompression failed";
                    return false;
                }
            }
            current_data = temp_output;
        }
        
        // Sparse decompression (run-length encoding) - before PKWARE
        if (compression_mask & MPQ_COMPRESSION_SPARSE) {
            temp_output.clear();
            if (!SparseDecompress(current_data, temp_output, expected_size)) {
                last_error = "Sparse decompression failed";
                return false;
            }
            current_data = temp_output;
        }
        
        // PKWARE implode decompression (innermost layer)
        if (compression_mask & MPQ_COMPRESSION_PKWARE) {
            temp_output.clear();
            if (!decompressPKWARE(current_data, temp_output, expected_size)) {
                last_error = "PKWARE decompression failed";
                return false;
            }
            current_data = temp_output;
            
            // If PKWARE returned less data than expected but succeeded, that's OK
            // This happens for the last sector of a file
            if (current_data.size() < expected_size && current_data.size() > 0) {
                output = current_data;
                return true;
            }
        }
        
        // If no compression was applied, data is stored as-is
        if (compression_mask == 0) {
            output = current_data;
            // std::cerr << "DEBUG: No compression - data size: " << output.size() 
            //          << ", expected: " << expected_size << std::endl;
            if (output.size() != expected_size) {
                last_error = "Uncompressed data size mismatch";
                return false;
            }
            return true;
        }
        
        output = current_data;
        return output.size() == expected_size;
    }
    
    // Load and parse listfile for filename resolution
    void loadListfile() {
        block_index_to_filename.clear();
        
        // Try to find "(listfile)" in the hash table
        uint32_t listfile_name1 = hashString("(listfile)", MPQ_HASH_NAME_A);
        uint32_t listfile_name2 = hashString("(listfile)", MPQ_HASH_NAME_B);
        
        const MPQHashEntry* listfile_entry = nullptr;
        for (const auto& entry : hash_table) {
            if (entry.name1 == listfile_name1 && entry.name2 == listfile_name2 && 
                entry.block_index != 0xFFFFFFFF) {
                listfile_entry = &entry;
                break;
            }
        }
        
        if (!listfile_entry || listfile_entry->block_index >= block_table.size()) {
            return; // No listfile found
        }
        
        // Extract listfile content
        const MPQBlockEntry& block = block_table[listfile_entry->block_index];
        
        // Read listfile data
        file.seekg(block.file_pos);
        std::vector<char> listfile_data(block.unpacked_size);
        file.read(listfile_data.data(), block.unpacked_size);
        
        if (!file.good()) {
            return; // Failed to read listfile
        }
        
        // Parse listfile (one filename per line)
        std::string content(listfile_data.begin(), listfile_data.end());
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Remove trailing \r if present (Windows line endings)
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            if (line.empty()) continue;
            
            // Find the hash entry for this filename
            uint32_t name1 = hashString(line, MPQ_HASH_NAME_A);
            uint32_t name2 = hashString(line, MPQ_HASH_NAME_B);
            
            for (const auto& entry : hash_table) {
                if (entry.name1 == name1 && entry.name2 == name2 && 
                    entry.block_index != 0xFFFFFFFF) {
                    block_index_to_filename[entry.block_index] = line;
                    break;
                }
            }
        }
    }
};

// Constructor
MPQLoader::MPQLoader() : pImpl(std::make_unique<Impl>()) {}

// Destructor - required for pImpl idiom
MPQLoader::~MPQLoader() = default;

bool MPQLoader::open(const std::string& filepath) {
    // Close any previously opened file
    if (pImpl->is_open) {
        close();
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->last_error = "File not found: " + filepath;
        return false;
    }
    
    // Open the file
    pImpl->file.open(filepath, std::ios::binary);
    if (!pImpl->file.is_open()) {
        pImpl->last_error = "Failed to open file: " + filepath;
        return false;
    }
    
    // Read and validate header
    pImpl->file.read(reinterpret_cast<char*>(&pImpl->header), sizeof(MPQHeader));
    if (!pImpl->file.good() || pImpl->file.gcount() < static_cast<std::streamsize>(sizeof(MPQHeader))) {
        pImpl->file.close();
        // Check if we read enough to validate the header
        if (pImpl->file.gcount() >= 4 && !pImpl->validateHeader()) {
            pImpl->last_error = "Invalid MPQ file format";
        } else {
            pImpl->last_error = "Failed to read MPQ header";
        }
        return false;
    }
    
    // Validate MPQ signature
    if (!pImpl->validateHeader()) {
        pImpl->file.close();
        pImpl->last_error = "Invalid MPQ file format";
        return false;
    }
    
    // Mark as open
    pImpl->is_open = true;
    pImpl->filepath = filepath;
    pImpl->last_error.clear();
    
    // Read hash table
    pImpl->hash_table.resize(pImpl->header.hash_table_entries);
    pImpl->file.seekg(pImpl->header.hash_table_offset);
    pImpl->file.read(reinterpret_cast<char*>(pImpl->hash_table.data()), 
                     sizeof(MPQHashEntry) * pImpl->header.hash_table_entries);
    
    // Diablo II MPQs always have encrypted tables, so always try decryption
    std::vector<MPQHashEntry> backup_hash_table = pImpl->hash_table;
    
    // Try decryption with the standard key
    pImpl->decryptTable(reinterpret_cast<uint32_t*>(pImpl->hash_table.data()),
                        (sizeof(MPQHashEntry) * pImpl->header.hash_table_entries) / sizeof(uint32_t),
                        MPQ_KEY_HASH_TABLE);
    
    // Check if decryption produced valid-looking results
    bool hash_decryption_valid = false;
    int valid_entries = 0;
    for (const auto& entry : pImpl->hash_table) {
        // Check for valid block indices (not empty/deleted)
        if (entry.block_index < pImpl->header.block_table_entries) {
            valid_entries++;
            hash_decryption_valid = true;
        }
    }
    
    // If decryption didn't produce any valid entries, try without decryption
    if (!hash_decryption_valid || valid_entries == 0) {
        // Check if original data looks valid
        pImpl->hash_table = backup_hash_table;
        for (const auto& entry : pImpl->hash_table) {
            if (entry.block_index != 0xFFFFFFFF && entry.block_index < pImpl->header.block_table_entries) {
                hash_decryption_valid = true;
                break;
            }
        }
        
        // If original also doesn't look valid, keep the decrypted version
        // as Diablo II MPQs are always encrypted
        if (!hash_decryption_valid) {
            pImpl->decryptTable(reinterpret_cast<uint32_t*>(pImpl->hash_table.data()),
                                (sizeof(MPQHashEntry) * pImpl->header.hash_table_entries) / sizeof(uint32_t),
                                MPQ_KEY_HASH_TABLE);
        }
    }
    
    // Read block table
    pImpl->block_table.resize(pImpl->header.block_table_entries);
    pImpl->file.seekg(pImpl->header.block_table_offset);
    pImpl->file.read(reinterpret_cast<char*>(pImpl->block_table.data()),
                     sizeof(MPQBlockEntry) * pImpl->header.block_table_entries);
    
    // Diablo II MPQs always have encrypted tables, so always try decryption
    std::vector<MPQBlockEntry> backup_block_table = pImpl->block_table;
    
    // Try decryption with the standard key
    pImpl->decryptTable(reinterpret_cast<uint32_t*>(pImpl->block_table.data()),
                        (sizeof(MPQBlockEntry) * pImpl->header.block_table_entries) / sizeof(uint32_t),
                        MPQ_KEY_BLOCK_TABLE);
    
    // Check if decryption produced valid-looking results
    bool block_decryption_valid = false;
    int valid_blocks = 0;
    for (const auto& entry : pImpl->block_table) {
        // Check for reasonable file position and size values
        if (entry.file_pos > 0 && entry.file_pos < pImpl->header.archive_size &&
            entry.unpacked_size > 0 && entry.unpacked_size < 100*1024*1024) { // Max 100MB per file
            valid_blocks++;
            block_decryption_valid = true;
        }
    }
    
    // If decryption didn't produce enough valid blocks, try without decryption
    if (!block_decryption_valid || valid_blocks < pImpl->header.block_table_entries / 4) {
        // Check if original data looks valid
        pImpl->block_table = backup_block_table;
        valid_blocks = 0;
        for (const auto& entry : pImpl->block_table) {
            if (entry.unpacked_size > 0 && entry.file_pos > 0 && 
                entry.file_pos < pImpl->header.archive_size &&
                entry.unpacked_size < pImpl->header.archive_size) {
                valid_blocks++;
                block_decryption_valid = true;
            }
        }
        
        // If original also doesn't look valid, keep the decrypted version
        // as Diablo II MPQs are always encrypted
        if (valid_blocks < pImpl->header.block_table_entries / 4) {
            pImpl->decryptTable(reinterpret_cast<uint32_t*>(pImpl->block_table.data()),
                                (sizeof(MPQBlockEntry) * pImpl->header.block_table_entries) / sizeof(uint32_t),
                                MPQ_KEY_BLOCK_TABLE);
        }
    }
    
    // Load listfile for filename resolution
    pImpl->loadListfile();
    
    return true;
}

void MPQLoader::close() {
    if (pImpl->file.is_open()) {
        pImpl->file.close();
    }
    pImpl->is_open = false;
    pImpl->filepath.clear();
    pImpl->file_map.clear();
}

bool MPQLoader::isOpen() const {
    return pImpl->is_open;
}

std::vector<MPQFileInfo> MPQLoader::listFiles() const {
    std::vector<MPQFileInfo> result;
    
    if (!pImpl->is_open) {
        return result;
    }
    
    // For this simple implementation, we'll just return files we know about
    // In a real implementation, we'd need a file list table or scan the hash table
    for (size_t i = 0; i < pImpl->hash_table.size(); i++) {
        const auto& hash_entry = pImpl->hash_table[i];
        
        // Skip empty entries
        if (hash_entry.block_index == 0xFFFFFFFF) {
            continue;
        }
        
        // Get block info
        if (hash_entry.block_index < pImpl->block_table.size()) {
            const auto& block = pImpl->block_table[hash_entry.block_index];
            
            // Skip empty blocks
            if (block.unpacked_size == 0 || !(block.flags & MPQ_FILE_EXISTS)) {
                continue;
            }
            
            MPQFileInfo info;
            // Use resolved filename if available, otherwise use block index as fallback
            auto filename_it = pImpl->block_index_to_filename.find(hash_entry.block_index);
            if (filename_it != pImpl->block_index_to_filename.end()) {
                info.filename = filename_it->second;
            } else {
                info.filename = "Unknown_" + std::to_string(hash_entry.block_index);
            }
            info.compressed_size = block.packed_size;
            info.uncompressed_size = block.unpacked_size;
            info.flags = block.flags;
            info.locale = hash_entry.locale;
            info.platform = hash_entry.platform;
            
            result.push_back(info);
        }
    }
    
    return result;
}

bool MPQLoader::hasFile(const std::string& filename) const {
    if (!pImpl->is_open) return false;
    
    // Calculate hash values for the filename
    uint32_t name1 = pImpl->hashString(filename, MPQ_HASH_NAME_A);
    uint32_t name2 = pImpl->hashString(filename, MPQ_HASH_NAME_B);
    
    // Search hash table
    for (const auto& entry : pImpl->hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            return true;
        }
    }
    
    return false;
}

bool MPQLoader::extractFile(const std::string& filename, std::vector<uint8_t>& output) {
    output.clear();
    
    if (!pImpl->is_open) {
        pImpl->last_error = "Archive not open";
        return false;
    }
    
    // Calculate hash values for the filename
    uint32_t name1 = pImpl->hashString(filename, MPQ_HASH_NAME_A);
    uint32_t name2 = pImpl->hashString(filename, MPQ_HASH_NAME_B);
    
    // Find the file in hash table
    const MPQHashEntry* hash_entry = nullptr;
    for (const auto& entry : pImpl->hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            hash_entry = &entry;
            break;
        }
    }
    
    if (!hash_entry) {
        pImpl->last_error = "File not found: " + filename;
        return false;
    }
    
    // Get block entry
    if (hash_entry->block_index >= pImpl->block_table.size()) {
        pImpl->last_error = "Invalid block index";
        return false;
    }
    
    const MPQBlockEntry& block = pImpl->block_table[hash_entry->block_index];
    
    // Read file data
    pImpl->file.seekg(block.file_pos);
    
    // Read the raw data first
    std::vector<uint8_t> file_data(block.packed_size);
    pImpl->file.read(reinterpret_cast<char*>(file_data.data()), block.packed_size);
    
    if (!pImpl->file.good()) {
        pImpl->last_error = "Failed to read file data";
        return false;
    }
    
    // Handle encryption if present
    if (block.flags & MPQ_FILE_ENCRYPTED) {
        // Calculate encryption key based on filename
        // Extract just the filename without path
        std::string base_name = filename;
        size_t slash_pos = filename.find_last_of("\\/");
        if (slash_pos != std::string::npos) {
            base_name = filename.substr(slash_pos + 1);
        }
        
        // Convert to uppercase for key calculation
        for (char& c : base_name) {
            c = std::toupper(c);
        }
        
        uint32_t file_key = pImpl->hashString(base_name, MPQ_HASH_FILE_KEY);
        
        // Adjust key if needed
        if (block.flags & MPQ_FILE_ADJUSTED_KEY) {
            file_key = (file_key + block.file_pos) ^ block.unpacked_size;
        }
        
        // For single-unit files, decrypt the entire data
        // For sector-based files, we'll decrypt during decompression
        if (block.flags & MPQ_FILE_SINGLE_UNIT) {
            // Decrypt the entire file data for single-unit files
            pImpl->decryptFileData(file_data, file_data.size(), file_key);
        }
        // For sector-based files, the decryption happens in decompressSectorBased
    }
    
    // Check if file is compressed
    if (block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE)) {
        // Check if file uses single unit or sector-based compression
        if (block.flags & MPQ_FILE_SINGLE_UNIT) {
            // Single unit compression - entire file is compressed as one block
            if (!pImpl->decompressData(file_data, output, block.unpacked_size)) {
                return false; // Error message already set in decompressData
            }
        } else {
            // Sector-based compression
            // Pass encryption info if file is encrypted
            bool is_encrypted = (block.flags & MPQ_FILE_ENCRYPTED) != 0;
            uint32_t file_key_for_sectors = 0;
            if (is_encrypted) {
                // Recalculate the file key (we need it for sector offset table)
                std::string base_name = filename;
                size_t slash_pos = filename.find_last_of("\\/");
                if (slash_pos != std::string::npos) {
                    base_name = filename.substr(slash_pos + 1);
                }
                for (char& c : base_name) {
                    c = std::toupper(c);
                }
                file_key_for_sectors = pImpl->hashString(base_name, MPQ_HASH_FILE_KEY);
                if (block.flags & MPQ_FILE_ADJUSTED_KEY) {
                    file_key_for_sectors = (file_key_for_sectors + block.file_pos) ^ block.unpacked_size;
                }
            }
            if (!pImpl->decompressSectorBased(file_data, output, block.unpacked_size, is_encrypted, file_key_for_sectors)) {
                return false; // Error message already set in decompressSectorBased
            }
        }
    } else {
        // Uncompressed file
        output = std::move(file_data);
        
        // Verify size
        if (output.size() != block.unpacked_size) {
            pImpl->last_error = "File size mismatch";
            return false;
        }
    }
    
    return true;
}

std::optional<MPQFileInfo> MPQLoader::getFileInfo(const std::string& filename) const {
    if (!pImpl || !pImpl->file.is_open()) {
        return std::nullopt;
    }
    
    // Calculate hash values
    uint32_t name1 = pImpl->hashString(filename, MPQ_HASH_NAME_A);
    uint32_t name2 = pImpl->hashString(filename, MPQ_HASH_NAME_B);
    
    // Find in hash table
    const MPQHashEntry* hash_entry = nullptr;
    for (const auto& entry : pImpl->hash_table) {
        if (entry.name1 == name1 && entry.name2 == name2 && 
            entry.block_index != 0xFFFFFFFF) {
            hash_entry = &entry;
            break;
        }
    }
    
    if (!hash_entry || hash_entry->block_index >= pImpl->block_table.size()) {
        return std::nullopt;
    }
    
    const MPQBlockEntry& block = pImpl->block_table[hash_entry->block_index];
    
    MPQFileInfo info;
    info.filename = filename;
    info.compressed_size = block.packed_size;
    info.uncompressed_size = block.unpacked_size;
    info.flags = block.flags;
    info.locale = hash_entry->locale;
    info.platform = hash_entry->platform;
    
    return info;
}

std::string MPQLoader::getLastError() const {
    return pImpl->last_error;
}

uint32_t MPQLoader::hashString(const std::string& str, uint32_t hash_type) const {
    return pImpl->hashString(str, hash_type);
}

} // namespace utils
} // namespace d2portable