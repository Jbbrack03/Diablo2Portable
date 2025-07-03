#include "onboarding/onboarding_wizard.h"
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace d2 {

class OnboardingWizard::Impl {
public:
    bool fileBrowserSupported = true;
    bool mpqSelectionEnabled = true;
    ProgressCallback progressCallback;
    std::vector<std::string> importedFiles;
    fs::path importDirectory = "vendor/mpq"; // Default location for imported files
};

OnboardingWizard::OnboardingWizard() : pImpl(std::make_unique<Impl>()) {}
OnboardingWizard::~OnboardingWizard() = default;

bool OnboardingWizard::showFileBrowser() {
    return pImpl->fileBrowserSupported;
}

bool OnboardingWizard::canSelectMPQFiles() {
    return pImpl->mpqSelectionEnabled;
}

bool OnboardingWizard::validateMPQFiles(const std::vector<std::string>& files) {
    if (files.empty()) return false;
    
    // Basic validation - just check if files exist
    for (const auto& file : files) {
        std::ifstream f(file);
        if (!f.good()) return false;
    }
    
    return true;
}

bool OnboardingWizard::importFiles(const std::vector<std::string>& files) {
    if (files.empty()) return false;
    
    // For now, just validate that the files exist
    // In a real implementation, we'd copy them to the game directory
    return validateMPQFiles(files);
}

void OnboardingWizard::setProgressCallback(const ProgressCallback& callback) {
    pImpl->progressCallback = callback;
}

bool OnboardingWizard::importWithProgress(const std::vector<std::string>& files) {
    if (!validateMPQFiles(files)) return false;
    
    // Calculate total size for progress tracking
    size_t totalSize = 0;
    for (const auto& file : files) {
        totalSize += fs::file_size(file);
    }
    
    size_t processedSize = 0;
    
    // Process each file
    for (size_t i = 0; i < files.size(); ++i) {
        const auto& file = files[i];
        size_t fileSize = fs::file_size(file);
        
        // Simulate processing the file in chunks for progress updates
        const size_t chunkSize = 256 * 1024; // 256KB chunks
        size_t fileProcessed = 0;
        
        std::ifstream input(file, std::ios::binary);
        if (!input) return false;
        
        while (fileProcessed < fileSize) {
            size_t toRead = std::min(chunkSize, fileSize - fileProcessed);
            
            // Simulate reading/processing
            std::vector<char> buffer(toRead);
            input.read(buffer.data(), toRead);
            
            fileProcessed += toRead;
            processedSize += toRead;
            
            // Report progress
            if (pImpl->progressCallback) {
                float progress = static_cast<float>(processedSize) / static_cast<float>(totalSize);
                pImpl->progressCallback(progress);
            }
        }
    }
    
    // Ensure we report 100% completion
    if (pImpl->progressCallback) {
        pImpl->progressCallback(1.0f);
    }
    
    return true;
}

FileCheckResult OnboardingWizard::checkRequiredFiles() const {
    FileCheckResult result;
    
    // List of required MPQ files for a complete D2 installation
    const std::vector<std::string> requiredFiles = {
        "d2data.mpq",
        "d2exp.mpq",
        "d2sfx.mpq",
        "d2music.mpq",
        "d2speech.mpq",
        "d2video.mpq"
    };
    
    // Check which files are present in the import directory
    for (const auto& file : requiredFiles) {
        fs::path filePath = pImpl->importDirectory / file;
        if (!fs::exists(filePath)) {
            result.missingFiles.push_back(file);
        }
    }
    
    result.allFilesPresent = result.missingFiles.empty();
    return result;
}

void OnboardingWizard::setImportDirectory(const std::string& path) {
    pImpl->importDirectory = path;
}

} // namespace d2