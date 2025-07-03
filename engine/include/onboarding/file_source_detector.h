#pragma once

#include <vector>
#include <string>
#include <memory>

namespace d2 {

enum class D2Version {
    CLASSIC,
    LORD_OF_DESTRUCTION,
    UNKNOWN
};

class D2Installation {
public:
    D2Installation();
    ~D2Installation();
    D2Installation(D2Installation&&);
    D2Installation& operator=(D2Installation&&);
    
    // Delete copy operations
    D2Installation(const D2Installation&) = delete;
    D2Installation& operator=(const D2Installation&) = delete;
    
    bool hasRequiredMPQs() const;
    D2Version getVersion() const;
    
    static D2Installation create(const std::string& path, bool hasMPQs, D2Version version);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

class FileSourceDetector {
public:
    std::vector<D2Installation> scanForInstallations(const std::vector<std::string>& searchPaths);
};

} // namespace d2