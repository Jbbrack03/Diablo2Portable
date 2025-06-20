#pragma once

#include <memory>
#include "game/entity.h"
#include "game/item.h"

namespace d2::game {

class DroppedItem : public Entity {
public:
    DroppedItem(std::shared_ptr<Item> item, const glm::vec2& position)
        : m_item(item) {
        position_ = position;
    }
    
    std::shared_ptr<Item> getItem() const { return m_item; }
    
private:
    std::shared_ptr<Item> m_item;
};

} // namespace d2::game