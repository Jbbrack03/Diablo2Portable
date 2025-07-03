#pragma once

#include <string>

namespace d2 {

struct ExtractionResult {
    bool success = false;
    int filesExtracted = 0;
    bool foundMPQFiles = false;
    int mpqFilesFound = 0;
};

class FileFormatHandler {
public:
    ExtractionResult extractFromISO(const std::string& isoPath, const std::string& outputDir);
    ExtractionResult extractFromInstaller(const std::string& installerPath, const std::string& outputDir);
    ExtractionResult extractFromArchive(const std::string& archivePath, const std::string& outputDir);
};

} // namespace d2