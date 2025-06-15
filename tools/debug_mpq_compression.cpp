#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

// MPQ compression flags
const uint8_t MPQ_COMPRESSION_HUFFMAN = 0x01;
const uint8_t MPQ_COMPRESSION_ZLIB = 0x02;
const uint8_t MPQ_COMPRESSION_PKWARE = 0x08;
const uint8_t MPQ_COMPRESSION_BZIP2 = 0x10;
const uint8_t MPQ_COMPRESSION_SPARSE = 0x20;
const uint8_t MPQ_COMPRESSION_ADPCM_MONO = 0x40;
const uint8_t MPQ_COMPRESSION_ADPCM_STEREO = 0x80;

void printCompressionMask(uint8_t mask) {
    std::cout << "Compression mask: 0x" << std::hex << (int)mask << std::dec << " = ";
    if (mask & MPQ_COMPRESSION_HUFFMAN) std::cout << "Huffman ";
    if (mask & MPQ_COMPRESSION_ZLIB) std::cout << "Zlib ";
    if (mask & MPQ_COMPRESSION_PKWARE) std::cout << "PKWARE ";
    if (mask & MPQ_COMPRESSION_BZIP2) std::cout << "BZip2 ";
    if (mask & MPQ_COMPRESSION_SPARSE) std::cout << "Sparse ";
    if (mask & MPQ_COMPRESSION_ADPCM_MONO) std::cout << "ADPCM-Mono ";
    if (mask & MPQ_COMPRESSION_ADPCM_STEREO) std::cout << "ADPCM-Stereo ";
    std::cout << "\n";
}

class DebugMPQLoader : public MPQLoader {
public:
    bool debugExtractFile(const std::string& filename) {
        // Get file block info
        uint32_t name1 = hashString(filename, 0x00);
        uint32_t name2 = hashString(filename, 0x100);
        
        std::cout << "File: " << filename << "\n";
        std::cout << "Hash1: 0x" << std::hex << name1 << "\n";
        std::cout << "Hash2: 0x" << std::hex << name2 << std::dec << "\n";
        
        // Try normal extraction
        std::vector<uint8_t> output;
        bool result = extractFile(filename, output);
        
        if (!result) {
            std::cout << "Extraction failed: " << getLastError() << "\n";
        } else {
            std::cout << "Extraction successful! Size: " << output.size() << " bytes\n";
        }
        
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <mpq_file> <file_to_extract>\n";
        return 1;
    }
    
    DebugMPQLoader loader;
    std::cout << "Opening MPQ: " << argv[1] << "\n";
    
    if (!loader.open(argv[1])) {
        std::cout << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return 1;
    }
    
    std::cout << "\n";
    loader.debugExtractFile(argv[2]);
    
    return 0;
}