#include "extraction/patch_system.h"
#include <fstream>
#include <regex>
#include <algorithm>

namespace d2 {

std::string extractVersionFromFilename(const std::string& filename) {
    // Look for version patterns like 114d, 113c, etc.
    std::regex version_regex("(\\d{3}[a-z])");
    std::smatch match;
    
    if (std::regex_search(filename, match, version_regex)) {
        std::string version_str = match[1];
        // Convert 114d to 1.14d
        if (version_str.length() == 4) {
            return version_str.substr(0, 1) + "." + version_str.substr(1);
        }
    }
    
    return "";
}

std::vector<PatchInfo> PatchSystem::detectPatches(const std::filesystem::path& directory) {
    std::vector<PatchInfo> patches;
    
    // Scan directory for patch files
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            
            // Check if it's a standalone patch MPQ
            if (filename == "patch.mpq" || filename == "d2patch.mpq") {
                // Verify it's actually an MPQ file by checking header
                std::ifstream file(entry.path(), std::ios::binary);
                char header[4];
                file.read(header, 4);
                
                if (header[0] == 'M' && header[1] == 'P' && header[2] == 'Q' && header[3] == 0x1A) {
                    PatchInfo patch;
                    patch.filename = filename;
                    patch.type = PatchType::STANDALONE_MPQ;
                    patch.version = extractVersionFromFilename(filename);
                    patches.push_back(patch);
                }
            }
            // Check if it's a patch executable
            else if (filename.find("Patch") != std::string::npos && 
                     (filename.size() >= 4 && 
                      (filename.substr(filename.size() - 4) == ".exe" || 
                       filename.substr(filename.size() - 4) == ".EXE"))) {
                // Check if it's a PE executable with embedded MPQ
                std::ifstream file(entry.path(), std::ios::binary);
                char dos_header[2];
                file.read(dos_header, 2);
                
                if (dos_header[0] == 'M' && dos_header[1] == 'Z') {
                    // It's a PE file, search for embedded MPQ
                    file.seekg(0, std::ios::end);
                    auto file_size = file.tellg();
                    
                    // Search for MPQ signature in the file
                    const size_t buffer_size = 4096;
                    char buffer[buffer_size];
                    bool found_mpq = false;
                    
                    for (size_t offset = 0; offset < static_cast<size_t>(file_size) && !found_mpq; offset += buffer_size - 3) {
                        file.seekg(offset);
                        file.read(buffer, buffer_size);
                        auto bytes_read = file.gcount();
                        
                        // Search for MPQ signature in buffer
                        for (size_t i = 0; i < static_cast<size_t>(bytes_read) - 3; ++i) {
                            if (buffer[i] == 'M' && buffer[i+1] == 'P' && 
                                buffer[i+2] == 'Q' && buffer[i+3] == 0x1A) {
                                found_mpq = true;
                                break;
                            }
                        }
                    }
                    
                    if (found_mpq) {
                        PatchInfo patch;
                        patch.filename = filename;
                        patch.type = PatchType::PATCH_EXECUTABLE;
                        patch.version = extractVersionFromFilename(filename);
                        patches.push_back(patch);
                    }
                }
            }
        }
    }
    
    return patches;
}

bool PatchSystem::extractPatchFromExecutable(const std::filesystem::path& exePath, const std::filesystem::path& outputPath) {
    std::ifstream input(exePath, std::ios::binary);
    if (!input.is_open()) {
        return false;
    }
    
    // Search for MPQ signature in the executable
    input.seekg(0, std::ios::end);
    auto file_size = input.tellg();
    
    const size_t buffer_size = 4096;
    char buffer[buffer_size];
    size_t mpq_offset = 0;
    bool found_mpq = false;
    
    for (size_t offset = 0; offset < static_cast<size_t>(file_size) && !found_mpq; offset += buffer_size - 3) {
        input.seekg(offset);
        input.read(buffer, buffer_size);
        auto bytes_read = input.gcount();
        
        // Search for MPQ signature in buffer
        for (size_t i = 0; i < static_cast<size_t>(bytes_read) - 3; ++i) {
            if (buffer[i] == 'M' && buffer[i+1] == 'P' && 
                buffer[i+2] == 'Q' && buffer[i+3] == 0x1A) {
                mpq_offset = offset + i;
                found_mpq = true;
                break;
            }
        }
    }
    
    if (!found_mpq) {
        return false;
    }
    
    // Read MPQ archive size from header
    input.seekg(mpq_offset + 8);
    uint32_t archive_size = 0;
    input.read(reinterpret_cast<char*>(&archive_size), sizeof(archive_size));
    
    // Validate archive size
    if (archive_size == 0 || archive_size > 1024 * 1024 * 1024) { // Max 1GB
        return false;
    }
    
    // Extract the MPQ to output file
    input.seekg(mpq_offset);
    std::ofstream output(outputPath, std::ios::binary);
    if (!output.is_open()) {
        return false;
    }
    
    // Copy MPQ data
    const size_t copy_buffer_size = 65536;
    char copy_buffer[copy_buffer_size];
    size_t remaining = archive_size;
    
    while (remaining > 0 && input.good()) {
        size_t to_read = std::min(remaining, copy_buffer_size);
        input.read(copy_buffer, to_read);
        size_t bytes_read = input.gcount();
        if (bytes_read == 0) break;
        
        output.write(copy_buffer, bytes_read);
        remaining -= bytes_read;
    }
    
    output.close();
    input.close();
    
    return remaining == 0;
}

} // namespace d2