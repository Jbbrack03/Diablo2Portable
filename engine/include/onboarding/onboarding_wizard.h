#pragma once

#include <memory>

namespace d2 {

class OnboardingWizard {
public:
    OnboardingWizard();
    ~OnboardingWizard();
    
    bool showFileBrowser();
    bool canSelectMPQFiles();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace d2