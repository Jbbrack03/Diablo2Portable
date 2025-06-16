#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>

// Simple MPQ header check
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>" << std::endl;
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    
    // Read first 32 bytes
    unsigned char header[32];
    file.read(reinterpret_cast<char*>(header), 32);
    
    std::cout << "First 32 bytes of file:" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                 << static_cast<int>(header[i]) << " ";
        if (i % 16 == 15) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Check MPQ signature
    if (header[0] == 'M' && header[1] == 'P' && header[2] == 'Q' && header[3] == 0x1A) {
        std::cout << "Valid MPQ signature found!" << std::endl;
        
        // Read some basic header info
        uint32_t header_size = *reinterpret_cast<uint32_t*>(&header[4]);
        uint32_t archive_size = *reinterpret_cast<uint32_t*>(&header[8]);
        uint16_t version = *reinterpret_cast<uint16_t*>(&header[12]);
        uint16_t block_size = *reinterpret_cast<uint16_t*>(&header[14]);
        
        std::cout << "Header size: " << header_size << std::endl;
        std::cout << "Archive size: " << archive_size << std::endl;
        std::cout << "Version: " << version << std::endl;
        std::cout << "Block size power: " << block_size << std::endl;
        std::cout << "Sector size: " << (512 << block_size) << " bytes" << std::endl;
    } else {
        std::cout << "Not a valid MPQ file!" << std::endl;
    }
    
    return 0;
}