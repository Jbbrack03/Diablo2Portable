#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace d2 {

struct HelpTopic {
    std::string id;
    std::string title;
    std::string content;
    std::vector<std::string> keywords;
    std::string category;
};

class HelpSystem {
public:
    HelpSystem() = default;
    ~HelpSystem() = default;

    // Load help topics
    void loadHelpTopics();

    // Topic management
    std::vector<HelpTopic> getAllTopics() const;
    bool hasTopic(const std::string& topicId) const;
    const HelpTopic* getTopic(const std::string& topicId) const;
    
    // Search functionality
    std::vector<HelpTopic> searchByKeyword(const std::string& keyword) const;
    std::vector<HelpTopic> getTopicsByCategory(const std::string& category) const;
    std::vector<std::string> getAllCategories() const;

private:
    std::unordered_map<std::string, HelpTopic> topics_;
};

} // namespace d2