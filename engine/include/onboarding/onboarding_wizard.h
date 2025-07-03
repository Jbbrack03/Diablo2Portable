#pragma once

#include <memory>
#include <vector>
#include <string>

namespace d2 {

class OnboardingWizard {
public:
    OnboardingWizard();
    ~OnboardingWizard();
    
    bool showFileBrowser();
    bool canSelectMPQFiles();
    
    bool validateMPQFiles(const std::vector<std::string>& files);
    bool importFiles(const std::vector<std::string>& files);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace d2