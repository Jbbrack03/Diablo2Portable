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
        }
    }
    
    return patches;
}

} // namespace d2