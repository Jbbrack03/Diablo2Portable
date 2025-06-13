#include <iostream>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include "../engine/include/utils/mpq_loader.h"

namespace fs = std::filesystem;
using namespace d2portable::utils;

void printFileInfo(const MPQFileInfo& info) {
    std::cout << "  File: " << info.filename << "\n";
    std::cout << "    Compressed size: " << info.compressed_size << " bytes\n";
    std::cout << "    Uncompressed size: " << info.uncompressed_size << " bytes\n";
    std::cout << "    Compression ratio: " << std::fixed << std::setprecision(2) 
              << (100.0 - (info.compressed_size * 100.0 / info.uncompressed_size)) << "%\n";
    std::cout << "    Flags: 0x" << std::hex << info.flags << std::dec << "\n";
}

void testMPQFile(const std::string& mpq_path) {
    std::cout << "\n=== Testing MPQ: " << mpq_path << " ===\n";
    
    // Get file size
    auto file_size = fs::file_size(mpq_path);
    std::cout << "File size: " << (file_size / 1024 / 1024) << " MB\n";
    
    MPQLoader loader;
    
    // Test opening
    auto start = std::chrono::high_resolution_clock::now();
    if (!loader.open(mpq_path)) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << "\n";
        return;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Opened successfully in " << duration.count() << "ms\n";
    
    // List files
    auto files = loader.listFiles();
    std::cout << "Total files: " << files.size() << "\n";
    
    // If no files found, try to extract by hash
    if (files.empty()) {
        std::cout << "\nNo files found via listfile. Trying direct hash extraction...\n";
        
        // Try to extract a known file by computing its hash
        std::string test_file = "data\\global\\palette\\pal.dat";
        if (loader.hasFile(test_file)) {
            std::cout << "File exists by hash lookup!\n";
            std::vector<uint8_t> data;
            if (loader.extractFile(test_file, data)) {
                std::cout << "Successfully extracted " << test_file << " (" << data.size() << " bytes)\n";
            }
        }
    }
    
    // Show first 5 files as examples
    int count = 0;
    for (const auto& file : files) {
        if (count++ >= 5) break;
        printFileInfo(file);
    }
    
    if (files.size() > 5) {
        std::cout << "  ... and " << (files.size() - 5) << " more files\n";
    }
    
    // Test extraction of a few common files
    std::vector<std::string> test_files = {
        "(listfile)",
        "data\\global\\palette\\pal.dat",
        "data\\global\\excel\\monstats.txt",
        "data\\global\\ui\\panel\\invchar6.dc6",
        // Also try with forward slashes
        "data/global/palette/pal.dat",
        // Try some other common files
        "data\\global\\music\\Act1\\town1.wav",
        "data\\local\\lng\\eng\\string.tbl"
    };
    
    std::cout << "\nTesting file extraction:\n";
    for (const auto& filename : test_files) {
        if (loader.hasFile(filename)) {
            std::vector<uint8_t> data;
            start = std::chrono::high_resolution_clock::now();
            if (loader.extractFile(filename, data)) {
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                auto info = loader.getFileInfo(filename);
                if (info) {
                    std::cout << "  ✓ " << filename << " - " 
                              << data.size() << " bytes extracted in " 
                              << duration.count() << "ms";
                    if (info->compressed_size < info->uncompressed_size) {
                        std::cout << " (was compressed)";
                    }
                    std::cout << "\n";
                }
            } else {
                std::cout << "  ✗ " << filename << " - extraction failed: " 
                          << loader.getLastError() << "\n";
            }
        } else {
            std::cout << "  - " << filename << " - not found\n";
        }
    }
    
    loader.close();
}

int main(int argc, char* argv[]) {
    std::cout << "MPQ Real File Test Program\n";
    std::cout << "==========================\n";
    
    if (argc > 1) {
        // Test specific MPQ file
        testMPQFile(argv[1]);
    } else {
        // Look for MPQ files in common locations
        std::vector<std::string> search_paths = {
            ".",
            "vendor",
            "vendor/mpq",
            "vendor/Diablo II",
            "/tmp/d2"
        };
        
        std::cout << "Searching for MPQ files...\n";
        
        for (const auto& path : search_paths) {
            if (fs::exists(path)) {
                for (const auto& entry : fs::directory_iterator(path)) {
                    if (entry.path().extension() == ".mpq" || 
                        entry.path().extension() == ".MPQ") {
                        testMPQFile(entry.path().string());
                    }
                }
            }
        }
        
        std::cout << "\nUsage: " << argv[0] << " <path_to_mpq>\n";
    }
    
    return 0;
}