#include <iostream>
#include <vector>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <path_to_mpq> <filename>\n";
        std::cerr << "Example: " << argv[0] << " d2data.mpq \"data\\\\global\\\\ui\\\\cursor\\\\ohand.dc6\"\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::string filename = argv[2];
    std::cout << "=== Testing extraction of: " << filename << " ===\n\n";
    
    // Check if file exists
    if (!loader.hasFile(filename)) {
        std::cout << "File not found in MPQ\n";
        return 1;
    }
    
    // Get file info
    auto file_info = loader.getFileInfo(filename);
    if (file_info) {
        std::cout << "File info:\n";
        std::cout << "  Compressed size: " << file_info->compressed_size << " bytes\n";
        std::cout << "  Uncompressed size: " << file_info->uncompressed_size << " bytes\n";
        std::cout << "  Flags: 0x" << std::hex << file_info->flags << std::dec << "\n";
        
        // Decode flags
        if (file_info->flags & 0x00000200) std::cout << "    - COMPRESSED\n";
        if (file_info->flags & 0x00010000) std::cout << "    - ENCRYPTED\n";
        if (file_info->flags & 0x01000000) std::cout << "    - SINGLE_UNIT\n";
        if (file_info->flags & 0x80000000) std::cout << "    - EXISTS\n";
    }
    
    // Try to extract
    std::cout << "\nAttempting extraction...\n";
    std::vector<uint8_t> data;
    if (loader.extractFile(filename, data)) {
        std::cout << "✅ Extraction successful! Got " << data.size() << " bytes\n";
        
        // Show first few bytes
        std::cout << "\nFirst 32 bytes of extracted data:\n";
        for (size_t i = 0; i < std::min(size_t(32), data.size()); i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << (int)data[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << std::dec << "\n";
        
        // Check DC6 header
        if (data.size() >= 24) {
            uint32_t version = *reinterpret_cast<const uint32_t*>(&data[0]);
            uint32_t flags = *reinterpret_cast<const uint32_t*>(&data[4]);
            uint32_t encoding = *reinterpret_cast<const uint32_t*>(&data[8]);
            
            std::cout << "\nDC6 Header:\n";
            std::cout << "  Version: " << version << " (should be 6)\n";
            std::cout << "  Flags: 0x" << std::hex << flags << std::dec << "\n";
            std::cout << "  Encoding: " << encoding << "\n";
        }
    } else {
        std::cout << "❌ Extraction failed: " << loader.getLastError() << "\n";
    }
    
    return 0;
}