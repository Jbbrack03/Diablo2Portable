#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <map>

namespace d2 {

enum class PatchType {
    STANDALONE_MPQ,
    PATCH_EXECUTABLE,
    INCREMENTAL_PATCH
};

enum class FileSourcePriority {
    BASE_GAME = 0,
    EXPANSION = 1,
    OFFICIAL_PATCH = 2,
    USER_MOD = 3
};

class PatchInfo {
public:
    std::string getFilename() const { return filename; }
    PatchType getType() const { return type; }
    std::string getVersion() const { return version; }
    
    std::string filename;
    PatchType type;
    std::string version;
};

class PatchSystem {
public:
    std::vector<PatchInfo> detectPatches(const std::filesystem::path& directory);
};

struct FileResolution {
    std::string source;
    FileSourcePriority priority;
};

class FilePrioritySystem {
public:
    void addSource(const std::string& name, FileSourcePriority priority) {
        sources[name] = priority;
    }
    
    void addFile(const std::string& source, const std::string& filepath) {
        files[filepath].push_back({source, sources[source]});
    }
    
    FileResolution resolveFile(const std::string& filepath) {
        auto it = files.find(filepath);
        if (it == files.end()) {
            return {"", FileSourcePriority::BASE_GAME};
        }
        
        // Find highest priority source
        FileResolution best = it->second[0];
        for (const auto& entry : it->second) {
            if (static_cast<int>(entry.priority) > static_cast<int>(best.priority)) {
                best = entry;
            }
        }
        
        return best;
    }
    
private:
    std::map<std::string, FileSourcePriority> sources;
    std::map<std::string, std::vector<FileResolution>> files;
};

} // namespace d2