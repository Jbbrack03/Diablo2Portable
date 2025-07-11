#include <iostream>
#include <vector>
#include <cstring>
#include "../engine/include/utils/pkware_explode.h"

using namespace d2portable::utils;

int main() {
    // Test simple PKWARE decompression
    std::string original = "Test content";
    
    // Create properly formatted PKWARE compressed data
    std::vector<uint8_t> compressed;
    
    // Add PKWARE header
    compressed.push_back(0); // CMP_BINARY
    compressed.push_back(4); // 4-bit dictionary size (minimum)
    
    // Add literal data (simplified - all literals)
    size_t pos = 0;
    while (pos < original.size()) {
        // Flag bit = 0 for literal
        uint8_t bits = 0;
        compressed.push_back(bits);
        
        // 8-bit literal
        if (pos < original.size()) {
            compressed.push_back(original[pos++]);
        }
    }
    
    std::cout << "Original: " << original << std::endl;
    std::cout << "Compressed size: " << compressed.size() << std::endl;
    
    // Try to decompress
    std::vector<uint8_t> output;
    bool result = PKWAREExplode(compressed, output, original.size());
    
    std::cout << "Decompression result: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    if (result) {
        std::string decompressed(output.begin(), output.end());
        std::cout << "Decompressed: " << decompressed << std::endl;
        std::cout << "Match: " << (decompressed == original ? "YES" : "NO") << std::endl;
    }
    
    return result ? 0 : 1;
}