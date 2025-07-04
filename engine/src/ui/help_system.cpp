#include "ui/help_system.h"
#include <algorithm>
#include <cctype>
#include <unordered_set>

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
    
    // Asset extraction topic
    HelpTopic assetExtraction;
    assetExtraction.id = "asset-extraction";
    assetExtraction.title = "Asset Extraction Guide";
    assetExtraction.content = "To extract game assets: 1) Select your D2 installation folder, 2) Choose MPQ files to extract, "
                              "3) Wait for extraction to complete. Supported formats: MPQ, ISO, CD drive. "
                              "The game requires d2data.mpq, d2exp.mpq, d2sfx.mpq, and d2music.mpq files.";
    assetExtraction.keywords = {"extract", "mpq", "assets", "files", "installation"};
    assetExtraction.category = "setup";
    topics_[assetExtraction.id] = assetExtraction;
    
    // Performance tips topic
    HelpTopic performanceTips;
    performanceTips.id = "performance-tips";
    performanceTips.title = "Performance Optimization";
    performanceTips.content = "For best performance: 1) Close background apps, 2) Lower graphics quality in settings, "
                              "3) Ensure device has sufficient free storage, 4) Restart the game if FPS drops. "
                              "Target: 60 FPS on Snapdragon processors.";
    performanceTips.keywords = {"performance", "fps", "lag", "slow", "optimization", "speed"};
    performanceTips.category = "support";
    topics_[performanceTips.id] = performanceTips;
    
    // Multiplayer setup topic
    HelpTopic multiplayerSetup;
    multiplayerSetup.id = "multiplayer-setup";
    multiplayerSetup.title = "Multiplayer Setup";
    multiplayerSetup.content = "To play multiplayer: 1) Connect to same WiFi network, 2) Host creates game, "
                               "3) Others join using host's IP address. Supports LAN play compatible with PC version. "
                               "Note: Internet play not supported.";
    multiplayerSetup.keywords = {"multiplayer", "network", "lan", "coop", "online"};
    multiplayerSetup.category = "gameplay";
    topics_[multiplayerSetup.id] = multiplayerSetup;
    
    // Save management topic
    HelpTopic saveManagement;
    saveManagement.id = "save-management";
    saveManagement.title = "Save Game Management";
    saveManagement.content = "Your saves are automatically stored. To manage saves: 1) Access from main menu, "
                             "2) Select character to load/delete, 3) Backup saves via file manager. "
                             "Save format: D2S compatible with PC version.";
    saveManagement.keywords = {"save", "load", "character", "backup", "restore"};
    saveManagement.category = "gameplay";
    topics_[saveManagement.id] = saveManagement;
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

std::vector<HelpTopic> HelpSystem::searchByKeyword(const std::string& keyword) const {
    std::vector<HelpTopic> results;
    
    // Convert keyword to lowercase for case-insensitive search
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    
    // Search through all topics
    for (const auto& [id, topic] : topics_) {
        bool found = false;
        
        // Check if keyword is in the topic's keywords
        for (const auto& topicKeyword : topic.keywords) {
            std::string lowerTopicKeyword = topicKeyword;
            std::transform(lowerTopicKeyword.begin(), lowerTopicKeyword.end(), lowerTopicKeyword.begin(), ::tolower);
            if (lowerTopicKeyword.find(lowerKeyword) != std::string::npos) {
                found = true;
                break;
            }
        }
        
        // Also check title and content
        if (!found) {
            std::string lowerTitle = topic.title;
            std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
            if (lowerTitle.find(lowerKeyword) != std::string::npos) {
                found = true;
            }
        }
        
        if (!found) {
            std::string lowerContent = topic.content;
            std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);
            if (lowerContent.find(lowerKeyword) != std::string::npos) {
                found = true;
            }
        }
        
        if (found) {
            results.push_back(topic);
        }
    }
    
    return results;
}

std::vector<HelpTopic> HelpSystem::getTopicsByCategory(const std::string& category) const {
    std::vector<HelpTopic> results;
    
    // Find all topics that match the given category
    for (const auto& [id, topic] : topics_) {
        if (topic.category == category) {
            results.push_back(topic);
        }
    }
    
    return results;
}

std::vector<std::string> HelpSystem::getAllCategories() const {
    std::vector<std::string> categories;
    std::unordered_set<std::string> uniqueCategories;
    
    // Collect unique categories from all topics
    for (const auto& [id, topic] : topics_) {
        uniqueCategories.insert(topic.category);
    }
    
    // Convert set to vector
    categories.reserve(uniqueCategories.size());
    for (const auto& category : uniqueCategories) {
        categories.push_back(category);
    }
    
    return categories;
}

} // namespace d2