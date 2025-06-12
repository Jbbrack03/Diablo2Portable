#include "game/item.h"

namespace d2::game {

Item::Item(const std::string& name, ItemType type)
    : m_name(name), m_type(type) {
}

} // namespace d2::game