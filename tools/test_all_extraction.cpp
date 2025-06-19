#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include "../engine/include/utils/stormlib_mpq_loader.h"

using namespace d2;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_d2data.mpq>\n";
        return 1;
    }
    
    std::cout << "=== Testing Extraction of ALL Files from MPQ ===\n\n";
    
    StormLibMPQLoader loader;
    if (!loader.open(argv[1])) {
        std::cerr << "Failed to open MPQ: " << loader.getLastError() << std::endl;
        return 1;
    }
    
    // Get list of all files
    auto files = loader.listFiles();
    std::cout << "Total files in MPQ: " << files.size() << "\n\n";
    
    // Track statistics
    int total_files = files.size();
    int successful = 0;
    int failed = 0;
    size_t total_extracted_size = 0;
    std::map<std::string, int> failures_by_type;
    std::vector<std::string> failed_files;
    
    // Track file types
    std::map<std::string, int> file_types;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Try to extract every file
    std::cout << "Extracting all files...\n";
    int progress_counter = 0;
    
    for (const auto& file_info : files) {
        // Get file extension
        std::string ext = "";
        size_t dot_pos = file_info.filename.rfind('.');
        if (dot_pos != std::string::npos) {
            ext = file_info.filename.substr(dot_pos);
        }
        file_types[ext]++;
        
        // Try to extract
        std::vector<uint8_t> data;
        if (loader.extractFile(file_info.filename, data)) {
            successful++;
            total_extracted_size += data.size();
        } else {
            failed++;
            failures_by_type[ext]++;
            failed_files.push_back(file_info.filename);
        }
        
        // Show progress every 100 files
        progress_counter++;
        if (progress_counter % 100 == 0) {
            std::cout << "\rProgress: " << progress_counter << "/" << total_files 
                      << " (" << (progress_counter * 100 / total_files) << "%)     " << std::flush;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "\n\n=== Extraction Results ===\n";
    std::cout << "Total files: " << total_files << "\n";
    std::cout << "✅ Successfully extracted: " << successful << " (" 
              << (successful * 100 / total_files) << "%)\n";
    std::cout << "❌ Failed to extract: " << failed << " (" 
              << (failed * 100 / total_files) << "%)\n";
    std::cout << "Total extracted size: " << (total_extracted_size / 1024 / 1024) << " MB\n";
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << "Average: " << (duration.count() / total_files) << " ms per file\n";
    
    // Show file types
    std::cout << "\n=== File Types ===\n";
    for (const auto& [ext, count] : file_types) {
        std::cout << std::setw(10) << (ext.empty() ? "(no ext)" : ext) 
                  << ": " << std::setw(5) << count << " files";
        if (failures_by_type.count(ext) > 0) {
            std::cout << " (❌ " << failures_by_type[ext] << " failed)";
        }
        std::cout << "\n";
    }
    
    // If there are failures, show first 10
    if (failed > 0) {
        std::cout << "\n=== Failed Files (first 10) ===\n";
        for (size_t i = 0; i < std::min(size_t(10), failed_files.size()); i++) {
            std::cout << "❌ " << failed_files[i] << "\n";
            
            // Get more info about why it failed
            auto info = loader.getFileInfo(failed_files[i]);
            if (info) {
                std::cout << "   Size: " << info->uncompressed_size 
                          << " (compressed: " << info->compressed_size << ")\n";
                std::cout << "   Flags: 0x" << std::hex << info->flags << std::dec << "\n";
            }
        }
        
        if (failed_files.size() > 10) {
            std::cout << "... and " << (failed_files.size() - 10) << " more\n";
        }
    }
    
    // Test specific known problematic files
    std::cout << "\n=== Testing Specific Files ===\n";
    std::vector<std::string> test_files = {
        "(listfile)",
        "(attributes)",
        "(signature)",
        "data\\global\\ui\\cursor\\ohand.dc6",
        "data\\global\\excel\\armor.txt",
        "data\\global\\sfx\\cursor\\button.wav"
    };
    
    for (const auto& file : test_files) {
        std::cout << "Testing: " << file << "\n";
        if (loader.hasFile(file)) {
            std::vector<uint8_t> data;
            if (loader.extractFile(file, data)) {
                std::cout << "  ✅ Extracted successfully (" << data.size() << " bytes)\n";
            } else {
                std::cout << "  ❌ Failed: " << loader.getLastError() << "\n";
            }
        } else {
            std::cout << "  ❌ File not found\n";
        }
    }
    
    std::cout << "\n=== Summary ===\n";
    if (failed == 0) {
        std::cout << "✅ ALL FILES EXTRACTED SUCCESSFULLY!\n";
    } else {
        std::cout << "⚠️  " << failed << " files failed to extract\n";
        std::cout << "Success rate: " << (successful * 100 / total_files) << "%\n";
    }
    
    return failed > 0 ? 1 : 0;
}