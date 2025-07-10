#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace d2 {

enum class PatchType {
    STANDALONE_MPQ,
    PATCH_EXECUTABLE,
    INCREMENTAL_PATCH
};

class PatchInfo {
public:
    std::string getFilename() const { return filename; }
    PatchType getType() const { return type; }
    
    std::string filename;
    PatchType type;
};

class PatchSystem {
public:
    std::vector<PatchInfo> detectPatches(const std::filesystem::path& directory);
};

} // namespace d2