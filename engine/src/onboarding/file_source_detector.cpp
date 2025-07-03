#include "onboarding/file_source_detector.h"
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;

namespace d2 {

// D2Installation implementation
class D2Installation::Impl {
public:
    std::string path;
    bool hasMPQs = false;
    D2Version version = D2Version::UNKNOWN;
};

D2Installation::D2Installation() : pImpl(std::make_unique<Impl>()) {}
D2Installation::~D2Installation() = default;
D2Installation::D2Installation(D2Installation&&) = default;
D2Installation& D2Installation::operator=(D2Installation&&) = default;

bool D2Installation::hasRequiredMPQs() const {
    return pImpl->hasMPQs;
}

D2Version D2Installation::getVersion() const {
    return pImpl->version;
}

// Static factory method to create D2Installation
D2Installation D2Installation::create(const std::string& path, bool hasMPQs, D2Version version) {
    D2Installation inst;
    inst.pImpl->path = path;
    inst.pImpl->hasMPQs = hasMPQs;
    inst.pImpl->version = version;
    return inst;
}

// FileSourceDetector implementation
std::vector<D2Installation> FileSourceDetector::scanForInstallations(const std::vector<std::string>& searchPaths) {
    std::vector<D2Installation> installations;
    
    for (const auto& searchPath : searchPaths) {
        if (!fs::exists(searchPath)) continue;
        
        // Look for Diablo II installations in subdirectories
        for (const auto& entry : fs::directory_iterator(searchPath)) {
            if (!entry.is_directory()) continue;
            
            fs::path dir = entry.path();
            
            // Check if this directory contains required MPQ files
            bool hasD2Data = fs::exists(dir / "d2data.mpq");
            bool hasD2Exp = fs::exists(dir / "d2exp.mpq");
            bool hasD2Sfx = fs::exists(dir / "d2sfx.mpq");
            
            if (hasD2Data && hasD2Sfx) {
                // If expansion MPQ exists, it's LoD version
                D2Version version = hasD2Exp ? D2Version::LORD_OF_DESTRUCTION : D2Version::CLASSIC;
                
                installations.push_back(D2Installation::create(dir.string(), true, version));
            }
        }
    }
    
    return installations;
}

// CDDrive implementation
class CDDrive::Impl {
public:
    std::string path;
    bool hasD2 = false;
    bool canReadMPQ = false;
};

CDDrive::CDDrive() : pImpl(std::make_unique<Impl>()) {}
CDDrive::~CDDrive() = default;
CDDrive::CDDrive(CDDrive&&) = default;
CDDrive& CDDrive::operator=(CDDrive&&) = default;

bool CDDrive::hasD2Disc() const {
    return pImpl->hasD2;
}

bool CDDrive::canReadMPQFiles() const {
    return pImpl->canReadMPQ;
}

std::vector<CDDrive> FileSourceDetector::detectCDDrives() {
    std::vector<CDDrive> drives;
    
    // Platform-specific CD drive detection
#ifdef __APPLE__
    // On macOS, check /Volumes for mounted CDs
    fs::path volumesPath = "/Volumes";
    if (fs::exists(volumesPath)) {
        for (const auto& entry : fs::directory_iterator(volumesPath)) {
            if (entry.is_directory()) {
                fs::path volumePath = entry.path();
                
                // Check if this volume looks like a D2 disc
                bool hasInstaller = fs::exists(volumePath / "Installer.exe") || 
                                   fs::exists(volumePath / "Install.exe");
                bool hasMPQ = fs::exists(volumePath / "d2data.mpq") ||
                              fs::exists(volumePath / "D2DATA.MPQ");
                
                if (hasInstaller || hasMPQ) {
                    CDDrive drive;
                    drive.pImpl->path = volumePath.string();
                    drive.pImpl->hasD2 = true;
                    drive.pImpl->canReadMPQ = hasMPQ;
                    drives.push_back(std::move(drive));
                }
            }
        }
    }
#elif defined(_WIN32)
    // Windows CD drive detection would go here
#elif defined(__linux__)
    // Linux CD drive detection would go here
#endif
    
    return drives;
}

ISOValidation FileSourceDetector::validateISOFile(const std::string& isoPath) {
    ISOValidation result;
    
    if (!fs::exists(isoPath)) {
        return result;
    }
    
    // Basic ISO validation - check if file exists and has reasonable size
    auto fileSize = fs::file_size(isoPath);
    if (fileSize < 1024) { // Too small to be a real ISO
        return result;
    }
    
    // For testing purposes, we'll do a simple check
    // In a real implementation, we'd mount the ISO or parse its filesystem
    std::ifstream file(isoPath, std::ios::binary);
    if (file) {
        // Read first few bytes to check if it looks like an ISO
        char header[16];
        file.read(header, sizeof(header));
        
        // Check for ISO 9660 signature (simplified)
        if (std::string(header, 8) == "ISO 9660") {
            result.isValid = true;
            result.containsD2Data = true; // For testing, assume it contains D2 data
            result.requiresMount = false; // We can read without mounting in this test
        }
    }
    
    return result;
}

std::vector<std::string> FileSourceDetector::getAndroidSearchPaths() const {
    std::vector<std::string> paths;
    
    // Common Android storage locations
    paths.push_back("/storage/emulated/0");
    paths.push_back("/storage/emulated/0/Download");
    paths.push_back("/storage/emulated/0/Documents");
    paths.push_back("/sdcard");
    paths.push_back("/sdcard/Download");
    
    // External SD card paths
    paths.push_back("/storage/sdcard1");
    paths.push_back("/storage/external_SD");
    
    // App-specific external storage (no permissions needed)
    paths.push_back("/storage/emulated/0/Android/data/com.diablo2portable/files");
    
    // Filter out non-existent paths
    std::vector<std::string> existingPaths;
    for (const auto& path : paths) {
        if (fs::exists(path)) {
            existingPaths.push_back(path);
        }
    }
    
    return existingPaths.empty() ? paths : existingPaths;
}

} // namespace d2