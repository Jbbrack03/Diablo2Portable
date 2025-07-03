#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace d2 {

class OnboardingWizard {
public:
    using ProgressCallback = std::function<void(float)>;
    
    OnboardingWizard();
    ~OnboardingWizard();
    
    bool showFileBrowser();
    bool canSelectMPQFiles();
    
    bool validateMPQFiles(const std::vector<std::string>& files);
    bool importFiles(const std::vector<std::string>& files);
    
    // Progress tracking support
    void setProgressCallback(const ProgressCallback& callback);
    bool importWithProgress(const std::vector<std::string>& files);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace d2