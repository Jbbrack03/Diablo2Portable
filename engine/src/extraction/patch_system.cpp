#include "extraction/patch_system.h"
#include <fstream>

namespace d2 {

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
                        patches.push_back(patch);
                    }
                }
            }
        }
    }
    
    return patches;
}

} // namespace d2