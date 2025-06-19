#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include "../engine/include/utils/stormlib_mpq_loader.h"

using namespace d2;
namespace fs = std::filesystem;

struct MPQStats {
    std::string filename;
    int total_files = 0;
    int extracted = 0;
    int failed = 0;
    size_t total_size = 0;
    std::map<std::string, int> file_types;
    std::vector<std::string> failed_files;
};

void processMP(const std::string& mpq_path, MPQStats& stats) {
    StormLibMPQLoader loader;
    
    std::cout << "\n=== Processing: " << fs::path(mpq_path).filename().string() << " ===\n";
    
    if (!loader.open(mpq_path)) {
        std::cout << "❌ Failed to open: " << loader.getLastError() << "\n";
        return;
    }
    
    auto files = loader.listFiles();
    stats.total_files = files.size();
    std::cout << "Found " << files.size() << " files\n";
    
    // Extract all files
    int progress = 0;
    for (const auto& file_info : files) {
        // Get file extension
        std::string ext = "";
        size_t dot_pos = file_info.filename.rfind('.');
        if (dot_pos != std::string::npos) {
            ext = file_info.filename.substr(dot_pos);
        }
        stats.file_types[ext]++;
        
        // Try to extract
        std::vector<uint8_t> data;
        if (loader.extractFile(file_info.filename, data)) {
            stats.extracted++;
            stats.total_size += data.size();
        } else {
            stats.failed++;
            stats.failed_files.push_back(file_info.filename);
        }
        
        // Progress
        progress++;
        if (progress % 100 == 0 || progress == stats.total_files) {
            std::cout << "\rProgress: " << progress << "/" << stats.total_files 
                      << " (" << (progress * 100 / stats.total_files) << "%)    " << std::flush;
        }
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_mpq_directory>\n";
        return 1;
    }
    
    std::string mpq_dir = argv[1];
    if (!fs::exists(mpq_dir) || !fs::is_directory(mpq_dir)) {
        std::cerr << "Invalid directory: " << mpq_dir << "\n";
        return 1;
    }
    
    std::cout << "=== Diablo II Complete MPQ Extraction Test ===\n";
    std::cout << "MPQ Directory: " << mpq_dir << "\n";
    
    // Find all MPQ files
    std::vector<std::string> mpq_files;
    for (const auto& entry : fs::directory_iterator(mpq_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mpq") {
            mpq_files.push_back(entry.path().string());
        }
    }
    
    std::cout << "\nFound " << mpq_files.size() << " MPQ files:\n";
    for (const auto& mpq : mpq_files) {
        std::cout << "  - " << fs::path(mpq).filename().string() << "\n";
    }
    
    // Process each MPQ
    std::vector<MPQStats> all_stats;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (const auto& mpq_path : mpq_files) {
        MPQStats stats;
        stats.filename = fs::path(mpq_path).filename().string();
        processMP(mpq_path, stats);
        all_stats.push_back(stats);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    
    // Summary
    std::cout << "\n\n=== COMPLETE EXTRACTION SUMMARY ===\n";
    
    int total_files = 0;
    int total_extracted = 0;
    int total_failed = 0;
    size_t total_size = 0;
    std::map<std::string, int> all_file_types;
    
    for (const auto& stats : all_stats) {
        std::cout << "\n" << stats.filename << ":\n";
        std::cout << "  Files: " << stats.total_files << "\n";
        std::cout << "  ✅ Extracted: " << stats.extracted << " (" 
                  << (stats.total_files > 0 ? stats.extracted * 100 / stats.total_files : 0) << "%)\n";
        std::cout << "  ❌ Failed: " << stats.failed << "\n";
        std::cout << "  Size: " << (stats.total_size / 1024 / 1024) << " MB\n";
        
        total_files += stats.total_files;
        total_extracted += stats.extracted;
        total_failed += stats.failed;
        total_size += stats.total_size;
        
        // Merge file types
        for (const auto& [ext, count] : stats.file_types) {
            all_file_types[ext] += count;
        }
        
        // Show failures if any
        if (!stats.failed_files.empty()) {
            std::cout << "  Failed files:\n";
            for (size_t i = 0; i < std::min(size_t(5), stats.failed_files.size()); i++) {
                std::cout << "    - " << stats.failed_files[i] << "\n";
            }
            if (stats.failed_files.size() > 5) {
                std::cout << "    ... and " << (stats.failed_files.size() - 5) << " more\n";
            }
        }
    }
    
    // Overall summary
    std::cout << "\n=== OVERALL STATISTICS ===\n";
    std::cout << "Total MPQ files processed: " << all_stats.size() << "\n";
    std::cout << "Total files: " << total_files << "\n";
    std::cout << "✅ Successfully extracted: " << total_extracted << " (" 
              << (total_files > 0 ? total_extracted * 100 / total_files : 0) << "%)\n";
    std::cout << "❌ Failed to extract: " << total_failed << "\n";
    std::cout << "Total extracted size: " << (total_size / 1024 / 1024) << " MB\n";
    std::cout << "Time taken: " << duration.count() << " seconds\n";
    
    // File types summary
    std::cout << "\n=== FILE TYPES ACROSS ALL MPQs ===\n";
    std::vector<std::pair<std::string, int>> sorted_types(all_file_types.begin(), all_file_types.end());
    std::sort(sorted_types.begin(), sorted_types.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& [ext, count] : sorted_types) {
        if (count > 10) { // Only show types with more than 10 files
            std::cout << std::setw(10) << (ext.empty() ? "(no ext)" : ext) 
                      << ": " << std::setw(6) << count << " files\n";
        }
    }
    
    // Check for specific important file types
    std::cout << "\n=== IMPORTANT FILE TYPES ===\n";
    std::cout << "Graphics:\n";
    std::cout << "  DC6 sprites: " << (all_file_types[".dc6"] + all_file_types[".DC6"]) << "\n";
    std::cout << "  DCC sprites: " << (all_file_types[".dcc"] + all_file_types[".DCC"]) << "\n";
    std::cout << "  PCX images: " << (all_file_types[".pcx"] + all_file_types[".PCX"]) << "\n";
    
    std::cout << "\nAudio:\n";
    std::cout << "  WAV files: " << (all_file_types[".wav"] + all_file_types[".WAV"]) << "\n";
    
    std::cout << "\nData:\n";
    std::cout << "  TXT tables: " << (all_file_types[".txt"] + all_file_types[".TXT"]) << "\n";
    std::cout << "  TBL strings: " << (all_file_types[".tbl"] + all_file_types[".TBL"]) << "\n";
    std::cout << "  BIN data: " << (all_file_types[".bin"] + all_file_types[".BIN"]) << "\n";
    
    std::cout << "\nMaps:\n";
    std::cout << "  DS1 maps: " << (all_file_types[".ds1"] + all_file_types[".DS1"]) << "\n";
    std::cout << "  DT1 tiles: " << (all_file_types[".dt1"] + all_file_types[".DT1"]) << "\n";
    
    // Final verdict
    std::cout << "\n=== FINAL VERDICT ===\n";
    if (total_failed == 0) {
        std::cout << "✅ PERFECT! All " << total_extracted << " files extracted successfully!\n";
    } else {
        std::cout << "⚠️  " << total_failed << " files failed to extract (";
        std::cout << (total_failed * 100 / total_files) << "% failure rate)\n";
    }
    
    return total_failed > 0 ? 1 : 0;
}