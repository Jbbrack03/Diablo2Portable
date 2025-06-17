#include <iostream>
#include <vector>
#include <iomanip>
#include <cstring>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

// We need to expose internals to debug
class MPQDebugLoader : public MPQLoader {
public:
    void debugFile(const std::string& filename) {
        // Find the file using hash
        uint32_t hash_a = hashString(filename, 0x100);
        uint32_t hash_b = hashString(filename, 0x200);
        
        std::cout << "Looking for file: " << filename << "\n";
        std::cout << "Hash A: 0x" << std::hex << hash_a << "\n";
        std::cout << "Hash B: 0x" << std::hex << hash_b << std::dec << "\n\n";
        
        // Find in hash table
        for (size_t i = 0; i < mpq.hash_table.size(); i++) {
            const auto& entry = mpq.hash_table[i];
            if (entry.file_hash_a == hash_a && entry.file_hash_b == hash_b) {
                std::cout << "Found at hash index " << i << "\n";
                std::cout << "Block index: " << entry.block_index << "\n";
                
                if (entry.block_index < mpq.block_table.size()) {
                    const auto& block = mpq.block_table[entry.block_index];
                    std::cout << "\nBlock info:\n";
                    std::cout << "  File position: " << block.file_position << "\n";
                    std::cout << "  Compressed size: " << block.compressed_size << "\n";
                    std::cout << "  Uncompressed size: " << block.uncompressed_size << "\n";
                    std::cout << "  Flags: 0x" << std::hex << block.flags << std::dec << "\n";
                    
                    // Decode flags
                    if (block.flags & 0x00000100) std::cout << "    - IMPLODE compression\n";
                    if (block.flags & 0x00000200) std::cout << "    - COMPRESSED\n";
                    if (block.flags & 0x00010000) std::cout << "    - ENCRYPTED\n";
                    if (block.flags & 0x00020000) std::cout << "    - FIX_KEY\n";
                    if (block.flags & 0x00100000) std::cout << "    - PATCH_FILE\n";
                    if (block.flags & 0x01000000) std::cout << "    - SINGLE_UNIT\n";
                    if (block.flags & 0x02000000) std::cout << "    - DELETE_MARKER\n";
                    if (block.flags & 0x04000000) std::cout << "    - SECTOR_CRC\n";
                    if (block.flags & 0x80000000) std::cout << "    - EXISTS\n";
                    
                    // Read first few bytes of compressed data
                    if (block.flags & 0x00000200) { // Compressed
                        std::cout << "\nReading compressed data...\n";
                        
                        // Seek to file position
                        file->seekg(mpq.header.offset + block.file_position);
                        
                        if (block.flags & 0x01000000) { // Single unit
                            std::cout << "Single unit compression\n";
                            
                            // Read compression mask
                            uint8_t compression_mask;
                            file->read(reinterpret_cast<char*>(&compression_mask), 1);
                            
                            std::cout << "Compression mask: 0x" << std::hex 
                                      << (int)compression_mask << std::dec << "\n";
                            
                            if (compression_mask & 0x01) std::cout << "  - Huffman\n";
                            if (compression_mask & 0x02) std::cout << "  - Zlib\n";
                            if (compression_mask & 0x08) std::cout << "  - PKWARE\n";
                            if (compression_mask & 0x10) std::cout << "  - BZip2\n";
                            if (compression_mask & 0x20) std::cout << "  - SPARSE\n";
                            if (compression_mask & 0x40) std::cout << "  - ADPCM (mono)\n";
                            if (compression_mask & 0x80) std::cout << "  - ADPCM (stereo)\n";
                            
                            // Read next few bytes
                            std::vector<uint8_t> sample(16);
                            file->read(reinterpret_cast<char*>(sample.data()), 16);
                            
                            std::cout << "\nNext 16 bytes after compression mask:\n";
                            for (int i = 0; i < 16; i++) {
                                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                          << (int)sample[i] << " ";
                            }
                            std::cout << std::dec << "\n";
                        } else {
                            std::cout << "Sector-based compression\n";
                            
                            // Read sector offset table
                            uint32_t sector_size = 0x1000;
                            uint32_t num_sectors = (block.uncompressed_size + sector_size - 1) / sector_size;
                            std::cout << "Number of sectors: " << num_sectors << "\n";
                            
                            // Read first sector offset
                            std::vector<uint32_t> sector_offsets(num_sectors + 1);
                            file->read(reinterpret_cast<char*>(sector_offsets.data()), 
                                      (num_sectors + 1) * sizeof(uint32_t));
                            
                            std::cout << "First few sector offsets:\n";
                            for (size_t i = 0; i < std::min(size_t(5), sector_offsets.size()); i++) {
                                std::cout << "  Sector " << i << ": " << sector_offsets[i] << "\n";
                            }
                            
                            // Read first sector data
                            if (num_sectors > 0) {
                                uint32_t first_sector_size = sector_offsets[1] - sector_offsets[0];
                                std::cout << "\nFirst sector size: " << first_sector_size << "\n";
                                
                                file->seekg(mpq.header.offset + block.file_position + sector_offsets[0]);
                                
                                // Read compression mask of first sector
                                uint8_t compression_mask;
                                file->read(reinterpret_cast<char*>(&compression_mask), 1);
                                
                                std::cout << "First sector compression mask: 0x" << std::hex 
                                          << (int)compression_mask << std::dec << "\n";
                                
                                if (compression_mask & 0x01) std::cout << "  - Huffman\n";
                                if (compression_mask & 0x02) std::cout << "  - Zlib\n";
                                if (compression_mask & 0x08) std::cout << "  - PKWARE\n";
                                if (compression_mask & 0x10) std::cout << "  - BZip2\n";
                                if (compression_mask & 0x20) std::cout << "  - SPARSE\n";
                            }
                        }
                    }
                }
                return;
            }
        }
        
        std::cout << "File not found in hash table\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <path_to_mpq> <filename>\n";
        std::cerr << "Example: " << argv[0] << " d2data.mpq \"data\\\\global\\\\ui\\\\cursor\\\\ohand.dc6\"\n";
        return 1;
    }
    
    MPQDebugLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    loader.debugFile(argv[2]);
    
    return 0;
}