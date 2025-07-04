#include "ui/help_system.h"

namespace d2 {

void HelpSystem::loadHelpTopics() {
    // Hardcoded help topics for now (like TutorialSystem)
    topics_.clear();
    
    // Getting Started topic
    HelpTopic gettingStarted;
    gettingStarted.id = "getting-started";
    gettingStarted.title = "Getting Started";
    gettingStarted.content = "To play Diablo II Portable, you need the original game files from your legally purchased copy of Diablo II.";
    gettingStarted.keywords = {"start", "begin", "setup", "install"};
    gettingStarted.category = "basics";
    topics_[gettingStarted.id] = gettingStarted;
    
    // Controls topic
    HelpTopic controls;
    controls.id = "controls";
    controls.title = "Game Controls";
    controls.content = "Use your gamepad to control your character. Left stick moves, buttons perform actions.";
    controls.keywords = {"controller", "gamepad", "input", "buttons"};
    controls.category = "basics";
    topics_[controls.id] = controls;
    
    // Troubleshooting topic
    HelpTopic troubleshooting;
    troubleshooting.id = "troubleshooting";
    troubleshooting.title = "Troubleshooting";
    troubleshooting.content = "Common issues: Missing files, permission denied, network failed, extraction failed.";
    troubleshooting.keywords = {"error", "problem", "issue", "fix"};
    troubleshooting.category = "support";
    topics_[troubleshooting.id] = troubleshooting;
}

std::vector<HelpTopic> HelpSystem::getAllTopics() const {
    std::vector<HelpTopic> result;
    result.reserve(topics_.size());
    
    for (const auto& [id, topic] : topics_) {
        result.push_back(topic);
    }
    
    return result;
}

bool HelpSystem::hasTopic(const std::string& topicId) const {
    return topics_.find(topicId) != topics_.end();
}

const HelpTopic* HelpSystem::getTopic(const std::string& topicId) const {
    auto it = topics_.find(topicId);
    if (it != topics_.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace d2