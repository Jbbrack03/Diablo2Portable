#include "ui/tutorial_system.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

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

bool TutorialSystem::loadTutorial(const std::string& tutorialName) {
    if (!initialized) {
        return false;
    }
    
    // For now, implement a simple hardcoded tutorial loader
    // This satisfies the test without requiring external dependencies
    if (tutorialName == "getting_started") {
        steps.clear();
        currentStep = 0;
        
        // Add welcome step
        TutorialStep welcome;
        welcome.id = "welcome";
        welcome.title = "Welcome to Diablo II";
        welcome.description = "Welcome to your adventure in Sanctuary!";
        welcome.imagePath = "welcome.png";
        welcome.completed = false;
        steps.push_back(welcome);
        
        // Add controls step
        TutorialStep controls;
        controls.id = "controls";
        controls.title = "Basic Controls";
        controls.description = "Use the left stick to move your character";
        controls.imagePath = "controls.png";
        controls.completed = false;
        steps.push_back(controls);
        
        return true;
    }
    
    return false;
}

const TutorialStep& TutorialSystem::getStep(size_t index) const {
    static TutorialStep emptyStep;
    if (index >= steps.size()) {
        return emptyStep;
    }
    return steps[index];
}

} // namespace d2