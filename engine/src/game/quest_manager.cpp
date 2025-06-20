#include "game/quest_manager.h"

namespace d2 {

std::shared_ptr<Quest> QuestManager::createQuest(QuestId id, const std::string& name, 
                                                 const std::string& description, QuestType type) {
    auto quest = std::make_shared<Quest>(id, name, description, type);
    quests_[id] = quest;
    return quest;
}

std::shared_ptr<Quest> QuestManager::getQuest(QuestId id) const {
    auto it = quests_.find(id);
    if (it != quests_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace d2