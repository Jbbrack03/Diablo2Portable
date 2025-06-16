#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2portable::utils;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    MPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== Debug MPQ Extraction Issues ===\n\n";
    
    // Get all files and find actual DC6 files
    auto files = loader.listFiles();
    std::cout << "Total files found: " << files.size() << std::endl;
    
    // Find all file extensions
    std::map<std::string, int> extensions;
    std::vector<std::string> dc6_files;
    
    for (const auto& fileInfo : files) {
        // Get extension
        size_t dot_pos = fileInfo.filename.find_last_of('.');
        if (dot_pos != std::string::npos) {
            std::string ext = fileInfo.filename.substr(dot_pos);
            extensions[ext]++;
        }
        
        if (fileInfo.filename.find(".dc6") != std::string::npos) {
            dc6_files.push_back(fileInfo.filename);
        }
    }
    
    std::cout << "File extensions found:\n";
    int count = 0;
    for (const auto& ext : extensions) {
        std::cout << "  " << ext.first << ": " << ext.second << " files\n";
        count++;
        if (count > 20) break; // Only show first 20 extensions
    }
    std::cout << std::endl;
    
    std::cout << "Found " << dc6_files.size() << " DC6 files\n\n";
    
    // Show first 10 DC6 files
    std::cout << "Sample DC6 files:\n";
    for (size_t i = 0; i < std::min((size_t)10, dc6_files.size()); i++) {
        std::cout << "  " << dc6_files[i] << std::endl;
    }
    std::cout << std::endl;
    
    // Test extraction of first few DC6 files
    std::cout << "Testing extraction:\n";
    int extraction_success = 0;
    int compression_errors = 0;
    
    for (size_t i = 0; i < std::min((size_t)5, dc6_files.size()); i++) {
        const std::string& filename = dc6_files[i];
        std::cout << "Testing: " << filename << std::endl;
        
        std::vector<uint8_t> data;
        if (loader.extractFile(filename, data)) {
            std::cout << "  ✅ Extracted " << data.size() << " bytes\n";
            extraction_success++;
        } else {
            std::cout << "  ❌ Extraction failed: " << loader.getLastError() << std::endl;
            if (loader.getLastError().find("compression") != std::string::npos ||
                loader.getLastError().find("PKWARE") != std::string::npos) {
                compression_errors++;
            }
        }
    }
    
    std::cout << "\nResults:\n";
    std::cout << "  Successful extractions: " << extraction_success << std::endl;
    std::cout << "  Compression errors: " << compression_errors << std::endl;
    
    // Test some simple text files first
    std::cout << "\nTesting text file extraction:\n";
    std::vector<std::string> text_files = {
        "data\\global\\excel\\armor.txt",
        "data\\global\\excel\\weapons.txt", 
        "data\\global\\excel\\misc.txt"
    };
    
    for (const auto& filename : text_files) {
        std::cout << "Testing: " << filename << std::endl;
        if (loader.hasFile(filename)) {
            std::vector<uint8_t> data;
            if (loader.extractFile(filename, data)) {
                std::cout << "  ✅ Extracted " << data.size() << " bytes\n";
                // Show first 100 bytes as text
                std::string preview(data.begin(), data.begin() + std::min((size_t)100, data.size()));
                std::cout << "  Preview: " << preview.substr(0, 50) << "...\n";
            } else {
                std::cout << "  ❌ Extraction failed: " << loader.getLastError() << std::endl;
            }
        } else {
            std::cout << "  ❌ File not found\n";
        }
    }
    
    return 0;
}