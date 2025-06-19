#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "utils/mpq_loader.h"

using namespace d2;

void analyzePKWAREData(const std::vector<uint8_t>& data) {
    std::cout << "\n=== PKWARE Data Analysis ===" << std::endl;
    std::cout << "Size: " << data.size() << " bytes" << std::endl;
    
    if (data.size() < 2) {
        std::cout << "ERROR: Data too small" << std::endl;
        return;
    }
    
    // Skip compression mask byte (already checked to be 0x08)
    const uint8_t* ptr = data.data() + 1;
    size_t remaining = data.size() - 1;
    
    std::cout << "\nFirst 32 bytes after compression mask (hex):" << std::endl;
    for (size_t i = 0; i < std::min(size_t(32), remaining); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                 << static_cast<int>(ptr[i]) << " ";
        if (i % 16 == 15) std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
    
    // PKWARE DCL header analysis
    if (remaining >= 1) {
        uint8_t header_byte = ptr[0];
        std::cout << "\nPKWARE DCL Header byte: 0x" << std::hex << (int)header_byte << std::dec << std::endl;
        
        // Decode header byte according to PKWARE DCL format
        int literal_size = (header_byte & 0x01) ? 8 : 7;  // 0 = ASCII (7-bit), 1 = Binary (8-bit)
        int dict_size_bits = (header_byte >> 1) & 0x03;   // 0 = 1K, 1 = 2K, 2 = 4K
        int dict_size = 1024 << dict_size_bits;
        
        std::cout << "  Literal size: " << literal_size << " bits" << std::endl;
        std::cout << "  Dictionary size: " << dict_size << " bytes" << std::endl;
        std::cout << "  Remaining bits: " << ((header_byte >> 3) & 0x1F) << std::endl;
    }
    
    // Look for patterns
    std::cout << "\nAnalyzing data patterns:" << std::endl;
    
    // Count byte frequency
    int byte_freq[256] = {0};
    for (size_t i = 0; i < remaining; ++i) {
        byte_freq[ptr[i]]++;
    }
    
    // Find most common bytes
    std::cout << "Most common bytes:" << std::endl;
    for (int count = 0; count < 5; ++count) {
        int max_freq = 0;
        int max_byte = 0;
        for (int i = 0; i < 256; ++i) {
            if (byte_freq[i] > max_freq) {
                max_freq = byte_freq[i];
                max_byte = i;
            }
        }
        if (max_freq > 0) {
            std::cout << "  0x" << std::hex << max_byte << std::dec 
                     << ": " << max_freq << " times (" 
                     << (max_freq * 100.0 / remaining) << "%)" << std::endl;
            byte_freq[max_byte] = 0;  // Clear for next iteration
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> <file_to_analyze>" << std::endl;
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << argv[1] << std::endl;
        return 1;
    }
    
    std::cout << "Attempting to analyze: " << argv[2] << std::endl;
    
    // We need to extract the raw compressed data before decompression
    // This would require modifying MPQLoader to expose this functionality
    // For now, let's try to extract and see what happens
    
    std::vector<uint8_t> data;
    bool success = loader.extractFile(argv[2], data);
    
    if (!success) {
        std::cout << "Extraction failed: " << loader.getLastError() << std::endl;
        
        // If it's PKWARE failure, we at least know it's using PKWARE
        if (loader.getLastError().find("PKWARE") != std::string::npos) {
            std::cout << "\nFile uses PKWARE compression but decompression failed." << std::endl;
            std::cout << "This suggests our PKWARE implementation needs fixing." << std::endl;
        }
    } else {
        std::cout << "Extraction succeeded! File size: " << data.size() << " bytes" << std::endl;
        
        // If it's a palette file, it should be 768 bytes (256 colors * 3 bytes)
        if (data.size() == 768) {
            std::cout << "This appears to be a valid palette file (256 colors)" << std::endl;
        }
    }
    
    return 0;
}