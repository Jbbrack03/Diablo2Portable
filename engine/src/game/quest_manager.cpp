#include "game/quest_manager.h"
#include <algorithm>

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

void QuestManager::startTracking(std::shared_ptr<Quest> quest) {
    if (quest && quest->isActive()) {
        activeQuests_.push_back(quest);
    }
}

void QuestManager::onMonsterKilled(game::MonsterType monsterType) {
    for (auto& quest : activeQuests_) {
        if (quest->isActive() && !quest->isComplete()) {
            quest->recordKill(monsterType);
        }
    }
}

bool QuestManager::isQuestComplete(QuestId id) const {
    auto quest = getQuest(id);
    return quest && quest->isComplete();
}

} // namespace d2