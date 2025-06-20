#include "game/quest.h"

namespace d2 {

Quest::Quest(QuestId id, const std::string& name, const std::string& description, QuestType type)
    : id_(id), name_(name), description_(description), type_(type) {
}

void Quest::setKillRequirement(game::MonsterType monsterType, int count) {
    requiredKills_[monsterType] = count;
}

int Quest::getRequiredKills(game::MonsterType monsterType) const {
    auto it = requiredKills_.find(monsterType);
    return (it != requiredKills_.end()) ? it->second : 0;
}

int Quest::getCurrentKills(game::MonsterType monsterType) const {
    auto it = currentKills_.find(monsterType);
    return (it != currentKills_.end()) ? it->second : 0;
}

void Quest::recordKill(game::MonsterType monsterType) {
    if (requiredKills_.find(monsterType) != requiredKills_.end()) {
        currentKills_[monsterType]++;
        checkCompletion();
    }
}

void Quest::checkCompletion() {
    if (type_ == QuestType::KILL_MONSTERS) {
        bool allKillsComplete = true;
        for (const auto& [monsterType, requiredCount] : requiredKills_) {
            if (getCurrentKills(monsterType) < requiredCount) {
                allKillsComplete = false;
                break;
            }
        }
        if (allKillsComplete) {
            isComplete_ = true;
        }
    }
}

} // namespace d2