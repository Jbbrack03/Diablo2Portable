#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace d2 {

enum class D2Version {
    CLASSIC,
    LORD_OF_DESTRUCTION,
    UNKNOWN
};

enum class NetworkType {
    SMB,
    FTP,
    HTTP
};

struct NetworkLocation {
    NetworkType type = NetworkType::SMB;
    std::string host;
    std::string share;
    std::string username;
    std::string password;
    int port = 0; // 0 means default port
};

struct NetworkConnectionResult {
    bool attempted = false;
    bool connected = false;
    std::string error;
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

class CDDrive {
public:
    CDDrive();
    ~CDDrive();
    CDDrive(CDDrive&&);
    CDDrive& operator=(CDDrive&&);
    
    // Delete copy operations
    CDDrive(const CDDrive&) = delete;
    CDDrive& operator=(const CDDrive&) = delete;
    
    bool hasD2Disc() const;
    bool canReadMPQFiles() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    friend class FileSourceDetector;
};

struct ISOValidation {
    bool isValid = false;
    bool containsD2Data = false;
    bool requiresMount = false;
};

class USBDevice {
public:
    USBDevice();
    ~USBDevice();
    USBDevice(USBDevice&&);
    USBDevice& operator=(USBDevice&&);
    
    // Delete copy operations
    USBDevice(const USBDevice&) = delete;
    USBDevice& operator=(const USBDevice&) = delete;
    
    std::string getPath() const;
    std::string getLabel() const;
    uint64_t getTotalSpace() const;
    uint64_t getFreeSpace() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    friend class FileSourceDetector;
};

class FileSourceDetector {
public:
    std::vector<D2Installation> scanForInstallations(const std::vector<std::string>& searchPaths);
    std::vector<CDDrive> detectCDDrives();
    ISOValidation validateISOFile(const std::string& isoPath);
    std::vector<USBDevice> detectUSBStorage();
    
    // Network location support
    NetworkConnectionResult connectToNetworkLocation(const NetworkLocation& location);
    std::vector<D2Installation> scanNetworkPath(const NetworkLocation& location, const std::string& path);
    
    // Platform-specific path helpers
    std::vector<std::string> getAndroidSearchPaths() const;
};

} // namespace d2