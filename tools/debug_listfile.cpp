#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "utils/mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file>" << std::endl;
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << argv[1] << std::endl;
        return 1;
    }
    
    std::cout << "=== Debugging (listfile) ===" << std::endl;
    
    // Check if listfile exists
    if (!loader.hasFile("(listfile)")) {
        std::cout << "(listfile) not found!" << std::endl;
        return 1;
    }
    
    // Get file info
    auto info = loader.getFileInfo("(listfile)");
    if (info) {
        std::cout << "File info:" << std::endl;
        std::cout << "  Compressed size: " << info->compressed_size << std::endl;
        std::cout << "  Uncompressed size: " << info->uncompressed_size << std::endl;
        std::cout << "  Flags: 0x" << std::hex << info->flags << std::dec << std::endl;
        
        // Decode flags
        const uint32_t MPQ_FILE_IMPLODE     = 0x00000100;
        const uint32_t MPQ_FILE_COMPRESS    = 0x00000200;
        const uint32_t MPQ_FILE_ENCRYPTED   = 0x00010000;
        const uint32_t MPQ_FILE_ADJUSTED_KEY = 0x00020000;
        const uint32_t MPQ_FILE_EXISTS      = 0x80000000;
        
        std::cout << "  Flags decoded:" << std::endl;
        if (info->flags & MPQ_FILE_EXISTS) std::cout << "    - EXISTS" << std::endl;
        if (info->flags & MPQ_FILE_IMPLODE) std::cout << "    - IMPLODE" << std::endl;
        if (info->flags & MPQ_FILE_COMPRESS) std::cout << "    - COMPRESS" << std::endl;
        if (info->flags & MPQ_FILE_ENCRYPTED) std::cout << "    - ENCRYPTED" << std::endl;
        if (info->flags & MPQ_FILE_ADJUSTED_KEY) std::cout << "    - ADJUSTED_KEY" << std::endl;
    }
    
    // Try to extract
    std::vector<uint8_t> data;
    bool success = loader.extractFile("(listfile)", data);
    
    if (!success) {
        std::cout << "\nExtraction failed: " << loader.getLastError() << std::endl;
    } else {
        std::cout << "\nExtraction successful!" << std::endl;
        std::cout << "Extracted size: " << data.size() << " bytes" << std::endl;
        
        // Show first few bytes
        std::cout << "First 32 bytes (hex):" << std::endl;
        for (size_t i = 0; i < std::min(size_t(32), data.size()); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(data[i]) << " ";
            if (i % 16 == 15) std::cout << std::endl;
        }
        std::cout << std::dec << std::endl;
        
        // Check if it's text
        bool is_text = true;
        for (size_t i = 0; i < std::min(size_t(100), data.size()); ++i) {
            if (data[i] < 32 && data[i] != '\n' && data[i] != '\r' && data[i] != '\t') {
                is_text = false;
                break;
            }
        }
        
        if (is_text) {
            std::cout << "\nAppears to be text. First few lines:" << std::endl;
            std::string content(data.begin(), data.end());
            size_t pos = 0;
            for (int i = 0; i < 5 && pos < content.size(); ++i) {
                size_t newline = content.find('\n', pos);
                if (newline == std::string::npos) newline = content.size();
                std::cout << "  " << content.substr(pos, newline - pos) << std::endl;
                pos = newline + 1;
            }
        }
    }
    
    return 0;
}