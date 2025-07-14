#include "extraction/patch_system.h"
#include "utils/file_utils.h"
#include <fstream>
#include <regex>
#include <algorithm>
#include <vector>
#include <iostream>

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
            if (filename.find(".mpq") != std::string::npos || filename.find(".MPQ") != std::string::npos) {
                // Verify it's actually an MPQ file by checking header
                size_t mpqOffset;
                if (d2::utils::FileUtils::findMPQSignature(entry.path().string(), mpqOffset) && mpqOffset == 0) {
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
                if (d2::utils::FileUtils::validateFileHeader(entry.path().string(), "MZ")) {
                    // It's a PE file, search for embedded MPQ
                    size_t mpqOffset;
                    if (d2::utils::FileUtils::findMPQSignature(entry.path().string(), mpqOffset)) {
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
    // Search for MPQ signature in the executable
    size_t mpq_offset;
    if (!d2::utils::FileUtils::findMPQSignature(exePath.string(), mpq_offset)) {
        return false;
    }
    
    std::ifstream input;
    if (!d2::utils::FileUtils::safeOpenBinaryFileForReading(exePath.string(), input)) {
        return false;
    }
    
    // Clear any potential stream error state and seek to archive size position
    input.clear();
    input.seekg(mpq_offset + 8);
    
    // Read MPQ archive size from header (32-bit little-endian)
    char size_bytes[4];
    input.read(size_bytes, 4);
    if (input.gcount() != 4) {
        return false;
    }
    
    uint32_t archive_size = *reinterpret_cast<uint32_t*>(size_bytes);
    
    // Validate archive size (reasonable bounds check)
    if (archive_size == 0 || archive_size > 1024 * 1024 * 1024) { // Max 1GB
        return false;
    }
    
    // Extract the MPQ to output file
    input.seekg(mpq_offset);
    std::ofstream output;
    if (!d2::utils::FileUtils::safeOpenBinaryFileForWriting(outputPath.string(), output)) {
        return false;
    }
    
    // Copy MPQ data
    std::vector<char> copy_buffer(archive_size);
    input.read(copy_buffer.data(), archive_size);
    auto bytes_read = input.gcount();
    
    if (bytes_read == static_cast<std::streamsize>(archive_size)) {
        output.write(copy_buffer.data(), bytes_read);
        d2::utils::FileUtils::safeCloseFile(output, outputPath.string());
        d2::utils::FileUtils::safeCloseFile(input, exePath.string());
        return true;
    }
    
    return false;
}

bool PatchSystem::applyPatch(const std::filesystem::path& baseMpq, const std::filesystem::path& patchMpq, const std::filesystem::path& outputDir) {
    // For now, just create the output directory
    try {
        std::filesystem::create_directories(outputDir);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> PatchSystem::getAvailableVersions(const std::filesystem::path& directory) {
    auto patches = detectPatches(directory);
    std::vector<std::string> versions;
    
    for (const auto& patch : patches) {
        if (!patch.version.empty()) {
            versions.push_back(patch.version);
        }
    }
    
    // Sort versions in descending order (newest first)
    std::sort(versions.begin(), versions.end(), [](const std::string& a, const std::string& b) {
        // Simple string comparison works for versions like "1.13c", "1.14d"
        return a > b;
    });
    
    return versions;
}

std::string PatchSystem::getLatestVersion(const std::filesystem::path& directory) {
    auto versions = getAvailableVersions(directory);
    return versions.empty() ? "" : versions[0];
}

} // namespace d2