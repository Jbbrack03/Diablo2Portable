#include "game/entity.h"

namespace d2::game {

EntityId Entity::nextId_ = 1;

Entity::Entity() : id_(nextId_++) {
}

} // namespace d2::game