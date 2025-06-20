#pragma once

#include <memory>
#include <unordered_map>
#include "game/quest.h"

namespace d2 {

class GameState;

class QuestManager {
public:
    QuestManager() = default;
    
    std::shared_ptr<Quest> createQuest(QuestId id, const std::string& name, 
                                       const std::string& description, QuestType type);
    
    std::shared_ptr<Quest> getQuest(QuestId id) const;
    
    // Quest tracking
    void startTracking(std::shared_ptr<Quest> quest);
    void onMonsterKilled(game::MonsterType monsterType);
    bool isQuestComplete(QuestId id) const;
    
private:
    std::unordered_map<QuestId, std::shared_ptr<Quest>> quests_;
    std::vector<std::shared_ptr<Quest>> activeQuests_;
};

} // namespace d2