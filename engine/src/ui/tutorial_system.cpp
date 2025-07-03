#include "ui/tutorial_system.h"
#include <filesystem>

namespace d2 {

bool TutorialSystem::initialize(const std::string& dataPath) {
    this->dataPath = dataPath;
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(dataPath)) {
        std::filesystem::create_directories(dataPath);
    }
    
    initialized = true;
    return true;
}

} // namespace d2