#include "onboarding/file_source_detector.h"
#include <filesystem>
#include <algorithm>

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

} // namespace d2