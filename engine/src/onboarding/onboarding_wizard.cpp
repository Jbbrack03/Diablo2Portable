#include "onboarding/onboarding_wizard.h"

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

} // namespace d2