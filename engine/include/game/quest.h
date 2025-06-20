#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "game/monster.h"

namespace d2 {

enum class QuestId {
    DEN_OF_EVIL,
    SISTERS_BURIAL_GROUNDS,
    BLOOD_RAVEN,
    CAIRN_STONES,
    FORGOTTEN_TOWER,
    SISTERS_TO_THE_SLAUGHTER,
    // Add more quest IDs as needed
};

enum class QuestType {
    KILL_MONSTERS,
    KILL_BOSS,
    FIND_ITEM,
    RESCUE_NPC,
    ACTIVATE_OBJECT
};

class Quest {
public:
    Quest(QuestId id, const std::string& name, const std::string& description, QuestType type);
    
    QuestId getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    QuestType getType() const { return type_; }
    
    bool isComplete() const { return isComplete_; }
    bool isActive() const { return isActive_; }
    
    void setComplete(bool complete) { isComplete_ = complete; }
    void setActive(bool active) { isActive_ = active; }
    
    // Kill tracking for quest objectives
    void setKillRequirement(game::MonsterType monsterType, int count);
    int getRequiredKills(game::MonsterType monsterType) const;
    int getCurrentKills(game::MonsterType monsterType) const;
    void recordKill(game::MonsterType monsterType);
    
private:
    QuestId id_;
    std::string name_;
    std::string description_;
    QuestType type_;
    bool isComplete_ = false;
    bool isActive_ = true;
    
    // Kill tracking
    std::unordered_map<game::MonsterType, int> requiredKills_;
    std::unordered_map<game::MonsterType, int> currentKills_;
    
    void checkCompletion();
};

} // namespace d2