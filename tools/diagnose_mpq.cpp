#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>
#include <map>
#include "utils/mpq_loader.h"

using namespace d2portable::utils;

// MPQ compression flags
const uint32_t MPQ_FILE_IMPLODE     = 0x00000100;
const uint32_t MPQ_FILE_COMPRESS    = 0x00000200;
const uint32_t MPQ_FILE_COMPRESSED  = 0x0000FF00;
const uint32_t MPQ_FILE_ENCRYPTED   = 0x00010000;
const uint32_t MPQ_FILE_EXISTS      = 0x80000000;

// Compression types
const uint8_t MPQ_COMPRESSION_HUFFMAN = 0x01;
const uint8_t MPQ_COMPRESSION_ZLIB    = 0x02;
const uint8_t MPQ_COMPRESSION_PKWARE  = 0x08;
const uint8_t MPQ_COMPRESSION_BZIP2   = 0x10;
const uint8_t MPQ_COMPRESSION_SPARSE  = 0x20;
const uint8_t MPQ_COMPRESSION_ADPCM_MONO   = 0x40;
const uint8_t MPQ_COMPRESSION_ADPCM_STEREO = 0x80;

std::string getCompressionTypes(uint32_t flags) {
    if (!(flags & MPQ_FILE_COMPRESS)) {
        return "None";
    }
    
    // For compressed files, we need to check the actual compression mask
    // This would require reading the first byte of the compressed data
    return "Compressed (need to check data)";
}

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
    
    std::cout << "=== MPQ Diagnostic Report ===" << std::endl;
    std::cout << "File: " << argv[1] << std::endl;
    
    auto files = loader.listFiles();
    std::cout << "\nTotal files: " << files.size() << std::endl;
    
    // Check for listfile
    std::cout << "\nChecking for (listfile)..." << std::endl;
    bool has_listfile = loader.hasFile("(listfile)");
    std::cout << "(listfile) present: " << (has_listfile ? "YES" : "NO") << std::endl;
    
    if (!has_listfile) {
        // Try uppercase
        has_listfile = loader.hasFile("(LISTFILE)");
        std::cout << "(LISTFILE) present: " << (has_listfile ? "YES" : "NO") << std::endl;
    }
    
    // Count file types
    int unknown_count = 0;
    int named_count = 0;
    std::map<std::string, int> extension_count;
    
    for (const auto& file : files) {
        if (file.filename.substr(0, 7) == "Unknown") {
            unknown_count++;
        } else {
            named_count++;
            
            // Get extension
            size_t dot_pos = file.filename.rfind('.');
            if (dot_pos != std::string::npos) {
                std::string ext = file.filename.substr(dot_pos);
                extension_count[ext]++;
            }
        }
    }
    
    std::cout << "\nFile naming status:" << std::endl;
    std::cout << "  Named files: " << named_count << std::endl;
    std::cout << "  Unknown files: " << unknown_count << std::endl;
    
    if (named_count > 0) {
        std::cout << "\nFile extensions found:" << std::endl;
        for (const auto& pair : extension_count) {
            std::cout << "  " << pair.first << ": " << pair.second << " files" << std::endl;
        }
    }
    
    // Try to manually find common files
    std::cout << "\nTrying to find common files:" << std::endl;
    const char* test_files[] = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt",
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\global\\palette\\act1\\pal.dat",
        "(listfile)",
        "(LISTFILE)",
        "(attributes)",
        "(signature)"
    };
    
    for (const auto& file : test_files) {
        bool exists = loader.hasFile(file);
        std::cout << "  " << file << ": " << (exists ? "FOUND" : "NOT FOUND") << std::endl;
        
        if (exists) {
            auto info = loader.getFileInfo(file);
            if (info) {
                std::cout << "    Compressed: " << info->compressed_size 
                         << ", Uncompressed: " << info->uncompressed_size
                         << ", Flags: 0x" << std::hex << info->flags << std::dec
                         << " (" << getCompressionTypes(info->flags) << ")" << std::endl;
            }
        }
    }
    
    // Show some actual filenames if available
    if (named_count > 0) {
        std::cout << "\nSample named files:" << std::endl;
        int shown = 0;
        for (const auto& file : files) {
            if (file.filename.substr(0, 7) != "Unknown") {
                std::cout << "  " << file.filename << std::endl;
                if (++shown >= 10) break;
            }
        }
    }
    
    return 0;
}