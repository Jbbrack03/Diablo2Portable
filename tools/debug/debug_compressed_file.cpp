#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include "utils/mpq_loader.h"

using namespace d2;

// Helper to show hex dump
void hexDump(const std::vector<uint8_t>& data, size_t offset, size_t length, const std::string& label) {
    std::cout << "\n" << label << " (offset " << offset << ", " << length << " bytes):" << std::endl;
    
    size_t end = std::min(offset + length, data.size());
    for (size_t i = offset; i < end; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                 << static_cast<int>(data[i]) << " ";
        if ((i - offset) % 16 == 15) std::cout << std::endl;
    }
    if ((end - offset) % 16 != 0) std::cout << std::endl;
    std::cout << std::dec;
}

// Helper to check if data looks like text
bool looksLikeText(const std::vector<uint8_t>& data, size_t offset, size_t length) {
    size_t end = std::min(offset + length, data.size());
    int printable = 0;
    int total = 0;
    
    for (size_t i = offset; i < end && total < 100; ++i, ++total) {
        uint8_t c = data[i];
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            printable++;
        }
    }
    
    return total > 0 && (printable * 100 / total) > 80;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mpq_file> <filename>" << std::endl;
        return 1;
    }
    
    std::string mpq_path = argv[1];
    std::string filename = argv[2];
    
    MPQLoader loader;
    if (!loader.open(mpq_path)) {
        std::cerr << "Failed to open MPQ: " << mpq_path << std::endl;
        return 1;
    }
    
    std::cout << "=== Analyzing '" << filename << "' ===" << std::endl;
    
    // Get file info
    auto info = loader.getFileInfo(filename);
    if (!info) {
        std::cerr << "File not found: " << filename << std::endl;
        return 1;
    }
    
    std::cout << "\nFile info:" << std::endl;
    std::cout << "  Compressed size: " << info->compressed_size << std::endl;
    std::cout << "  Uncompressed size: " << info->uncompressed_size << std::endl;
    std::cout << "  Flags: 0x" << std::hex << info->flags << std::dec << std::endl;
    
    // Try manual extraction to analyze the raw compressed data
    // We'll need to replicate some of the MPQLoader logic to see the raw data
    
    // For now, let's try to extract and see what happens
    std::vector<uint8_t> data;
    bool success = loader.extractFile(filename, data);
    
    if (success) {
        std::cout << "\nExtraction successful!" << std::endl;
        std::cout << "Extracted size: " << data.size() << " bytes" << std::endl;
        
        // Show the data
        hexDump(data, 0, 64, "Extracted data");
        
        if (looksLikeText(data, 0, 100)) {
            std::cout << "\nData appears to be text. First few lines:" << std::endl;
            std::string text(data.begin(), data.end());
            size_t pos = 0;
            for (int i = 0; i < 5 && pos < text.size(); ++i) {
                size_t newline = text.find('\n', pos);
                if (newline == std::string::npos) newline = text.size();
                std::cout << "  " << text.substr(pos, newline - pos) << std::endl;
                pos = newline + 1;
            }
        }
    } else {
        std::cout << "\nExtraction failed: " << loader.getLastError() << std::endl;
        
        // Let's try to understand why
        // The compression mask 0xd6 might be incorrect
        // Let's check if the file might be using a different compression format
        
        std::cout << "\nPossible issues:" << std::endl;
        std::cout << "1. Compression mask 0xd6 suggests multiple compression types including ADPCM" << std::endl;
        std::cout << "2. This is unusual for a text file like (listfile)" << std::endl;
        std::cout << "3. Possible explanations:" << std::endl;
        std::cout << "   - The first byte after decryption might not be the compression mask" << std::endl;
        std::cout << "   - The file might use a different encryption method" << std::endl;
        std::cout << "   - The compression format might be different than expected" << std::endl;
        
        // Let's check what compression types are in 0xd6
        uint8_t mask = 0xd6;
        std::cout << "\nCompression mask 0xd6 breakdown:" << std::endl;
        std::cout << "  Binary: " << std::bitset<8>(mask) << std::endl;
        if (mask & 0x01) std::cout << "  - 0x01: Huffman" << std::endl;
        if (mask & 0x02) std::cout << "  - 0x02: Zlib" << std::endl;
        if (mask & 0x04) std::cout << "  - 0x04: Unknown (not standard MPQ)" << std::endl;
        if (mask & 0x08) std::cout << "  - 0x08: PKWARE" << std::endl;
        if (mask & 0x10) std::cout << "  - 0x10: BZip2" << std::endl;
        if (mask & 0x20) std::cout << "  - 0x20: Sparse" << std::endl;
        if (mask & 0x40) std::cout << "  - 0x40: ADPCM Mono" << std::endl;
        if (mask & 0x80) std::cout << "  - 0x80: ADPCM Stereo" << std::endl;
    }
    
    return 0;
}