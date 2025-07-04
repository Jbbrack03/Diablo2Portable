#pragma once

#include <string>
#include <vector>
#include <functional>

namespace d2 {

/**
 * TutorialStep - Represents a single step in the tutorial
 */
struct TutorialStep {
    std::string id;
    std::string title;
    std::string description;
    std::string imagePath;
    bool completed = false;
};

/**
 * TutorialSystem - Interactive tutorial system for first-time users
 * 
 * This class provides step-by-step guidance for new users, including
 * onboarding tutorials, controller setup guides, and gameplay tips.
 */
class TutorialSystem {
public:
    TutorialSystem() = default;
    ~TutorialSystem() = default;
    
    /**
     * Initialize the tutorial system
     * @param dataPath Path to tutorial data directory
     * @return true if initialization succeeded
     */
    bool initialize(const std::string& dataPath);
    
    /**
     * Check if the tutorial system is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return initialized; }
    
    /**
     * Get the total number of tutorial steps
     * @return Number of steps in the current tutorial
     */
    size_t getTotalSteps() const { return steps.size(); }
    
    /**
     * Get the current step index
     * @return Current step index (0-based)
     */
    size_t getCurrentStep() const { return currentStep; }
    
    /**
     * Load a tutorial by name
     * @param tutorialName Name of the tutorial to load
     * @return true if tutorial was loaded successfully
     */
    bool loadTutorial(const std::string& tutorialName);
    
    /**
     * Get a specific tutorial step
     * @param index Step index
     * @return Tutorial step at the given index
     */
    const TutorialStep& getStep(size_t index) const;
    
private:
    bool initialized = false;
    std::string dataPath;
    std::vector<TutorialStep> steps;
    size_t currentStep = 0;
};

} // namespace d2