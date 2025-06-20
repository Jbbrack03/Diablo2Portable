#include "game/quest.h"

namespace d2 {

Quest::Quest(QuestId id, const std::string& name, const std::string& description, QuestType type)
    : id_(id), name_(name), description_(description), type_(type) {
}

} // namespace d2