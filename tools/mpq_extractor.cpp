#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "../engine/include/utils/mpq_loader.h"

using namespace d2;
namespace fs = std::filesystem;

void printUsage(const char* programName) {
    std::cout << "MPQ Extractor Tool\n";
    std::cout << "Usage: " << programName << " <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  list <mpq_file>                    List all files in the MPQ\n";
    std::cout << "  extract <mpq_file> <file> [output] Extract a single file\n";
    std::cout << "  extractall <mpq_file> <output_dir> Extract all files\n";
    std::cout << "  info <mpq_file>                    Show MPQ information\n";
    std::cout << "  search <mpq_file> <pattern>        Search for files matching pattern\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " list d2data.mpq\n";
    std::cout << "  " << programName << " extract d2data.mpq \"data\\global\\ui\\cursor\\cursor.dc6\"\n";
    std::cout << "  " << programName << " extractall d2data.mpq ./extracted/\n";
    std::cout << "  " << programName << " search d2data.mpq \"*.dc6\"\n";
}

void listFiles(const std::string& mpqPath) {
    MPQLoader loader;
    
    std::cout << "Opening MPQ: " << mpqPath << "\n";
    if (!loader.open(mpqPath)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    auto files = loader.listFiles();
    std::cout << "\nTotal files: " << files.size() << "\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left;
    std::cout << std::setw(50) << "Filename" 
              << std::setw(12) << "Size" 
              << std::setw(12) << "Compressed"
              << "Flags\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& file : files) {
        std::cout << std::setw(50) << file.filename;
        std::cout << std::setw(12) << file.uncompressed_size;
        std::cout << std::setw(12) << file.compressed_size;
        
        // Show flags
        if (file.flags & 0x00000200) std::cout << "C"; // Compressed
        if (file.flags & 0x00010000) std::cout << "E"; // Encrypted
        if (file.flags & 0x00000100) std::cout << "I"; // Implode (PKWARE)
        
        std::cout << "\n";
    }
}

void extractFile(const std::string& mpqPath, const std::string& fileName, 
                const std::string& outputPath = "") {
    MPQLoader loader;
    
    if (!loader.open(mpqPath)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    std::vector<uint8_t> data;
    std::cout << "Extracting: " << fileName << "\n";
    
    if (!loader.extractFile(fileName, data)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    // Determine output file path
    std::string outFile = outputPath;
    if (outFile.empty()) {
        // Use original filename, replacing backslashes
        outFile = fileName;
        std::replace(outFile.begin(), outFile.end(), '\\', '/');
        
        // Extract just the filename if it has a path
        size_t lastSlash = outFile.find_last_of('/');
        if (lastSlash != std::string::npos) {
            outFile = outFile.substr(lastSlash + 1);
        }
    }
    
    // Create directory if needed
    fs::path outPath(outFile);
    if (outPath.has_parent_path()) {
        fs::create_directories(outPath.parent_path());
    }
    
    // Write file
    std::ofstream out(outFile, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot create output file: " << outFile << "\n";
        return;
    }
    
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    std::cout << "Extracted " << data.size() << " bytes to: " << outFile << "\n";
}

void extractAll(const std::string& mpqPath, const std::string& outputDir) {
    MPQLoader loader;
    
    if (!loader.open(mpqPath)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    // Create output directory
    fs::create_directories(outputDir);
    
    auto files = loader.listFiles();
    std::cout << "Extracting " << files.size() << " files...\n";
    
    int extracted = 0;
    int failed = 0;
    
    for (const auto& fileInfo : files) {
        if (fileInfo.filename.empty() || fileInfo.filename.find("Unknown_") == 0) {
            continue; // Skip unknown files
        }
        
        std::vector<uint8_t> data;
        if (loader.extractFile(fileInfo.filename, data)) {
            // Convert path separators
            std::string outPath = fileInfo.filename;
            std::replace(outPath.begin(), outPath.end(), '\\', '/');
            
            // Full output path
            fs::path fullPath = fs::path(outputDir) / outPath;
            
            // Create directories
            fs::create_directories(fullPath.parent_path());
            
            // Write file
            std::ofstream out(fullPath, std::ios::binary);
            if (out) {
                out.write(reinterpret_cast<const char*>(data.data()), data.size());
                extracted++;
                
                if (extracted % 100 == 0) {
                    std::cout << "Extracted: " << extracted << " files\n";
                }
            } else {
                std::cerr << "Failed to write: " << fullPath << "\n";
                failed++;
            }
        } else {
            std::cerr << "Failed to extract: " << fileInfo.filename << "\n";
            failed++;
        }
    }
    
    std::cout << "\nExtraction complete!\n";
    std::cout << "Extracted: " << extracted << " files\n";
    std::cout << "Failed: " << failed << " files\n";
}

void showInfo(const std::string& mpqPath) {
    MPQLoader loader;
    
    if (!loader.open(mpqPath)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    auto files = loader.listFiles();
    
    // Calculate statistics
    size_t totalUncompressed = 0;
    size_t totalCompressed = 0;
    int compressedCount = 0;
    int encryptedCount = 0;
    
    for (const auto& file : files) {
        totalUncompressed += file.uncompressed_size;
        totalCompressed += file.compressed_size;
        
        if (file.flags & 0x00000200) compressedCount++;
        if (file.flags & 0x00010000) encryptedCount++;
    }
    
    std::cout << "\nMPQ Information: " << mpqPath << "\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << "Total files: " << files.size() << "\n";
    std::cout << "Compressed files: " << compressedCount << "\n";
    std::cout << "Encrypted files: " << encryptedCount << "\n";
    std::cout << "Total size (uncompressed): " << (totalUncompressed / 1024 / 1024) << " MB\n";
    std::cout << "Total size (compressed): " << (totalCompressed / 1024 / 1024) << " MB\n";
    
    if (totalUncompressed > 0) {
        double ratio = 100.0 * (1.0 - (double)totalCompressed / totalUncompressed);
        std::cout << "Compression ratio: " << std::fixed << std::setprecision(1) 
                  << ratio << "%\n";
    }
}

void searchFiles(const std::string& mpqPath, const std::string& pattern) {
    MPQLoader loader;
    
    if (!loader.open(mpqPath)) {
        std::cerr << "Error: " << loader.getLastError() << "\n";
        return;
    }
    
    auto files = loader.listFiles();
    int matches = 0;
    
    std::cout << "\nSearching for: " << pattern << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& file : files) {
        // Simple pattern matching (just suffix for now)
        bool match = false;
        if (pattern.front() == '*') {
            // Suffix match
            std::string suffix = pattern.substr(1);
            match = file.filename.size() >= suffix.size() &&
                    file.filename.substr(file.filename.size() - suffix.size()) == suffix;
        } else if (pattern.back() == '*') {
            // Prefix match
            std::string prefix = pattern.substr(0, pattern.size() - 1);
            match = file.filename.substr(0, prefix.size()) == prefix;
        } else {
            // Substring match
            match = file.filename.find(pattern) != std::string::npos;
        }
        
        if (match) {
            std::cout << std::setw(50) << std::left << file.filename;
            std::cout << std::setw(12) << std::right << file.uncompressed_size;
            std::cout << " bytes\n";
            matches++;
        }
    }
    
    std::cout << std::string(80, '-') << "\n";
    std::cout << "Found " << matches << " matching files\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    std::string mpqFile = argv[2];
    
    // Check if MPQ file exists
    if (!fs::exists(mpqFile)) {
        std::cerr << "Error: MPQ file not found: " << mpqFile << "\n";
        return 1;
    }
    
    try {
        if (command == "list") {
            listFiles(mpqFile);
        } else if (command == "extract" && argc >= 4) {
            std::string fileName = argv[3];
            std::string outputPath = (argc >= 5) ? argv[4] : "";
            extractFile(mpqFile, fileName, outputPath);
        } else if (command == "extractall" && argc >= 4) {
            std::string outputDir = argv[3];
            extractAll(mpqFile, outputDir);
        } else if (command == "info") {
            showInfo(mpqFile);
        } else if (command == "search" && argc >= 4) {
            std::string pattern = argv[3];
            searchFiles(mpqFile, pattern);
        } else {
            std::cerr << "Error: Invalid command or missing arguments\n\n";
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}