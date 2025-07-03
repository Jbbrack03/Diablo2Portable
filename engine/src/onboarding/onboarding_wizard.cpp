#include "onboarding/onboarding_wizard.h"
#include <fstream>
#include <vector>
#include <string>

namespace d2 {

class OnboardingWizard::Impl {
public:
    bool fileBrowserSupported = true;
    bool mpqSelectionEnabled = true;
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

} // namespace d2