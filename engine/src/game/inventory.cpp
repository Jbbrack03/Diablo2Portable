#include "game/inventory.h"
#include "game/item.h"

namespace d2::game {

Inventory::Inventory(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_grid(height, std::vector<std::shared_ptr<Item>>(width)) {
}

bool Inventory::addItem(std::shared_ptr<Item> item, int x, int y) {
    if (!item) return false;
    
    // Check if position is valid
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return false;
    }
    
    int itemWidth = item->getWidth();
    int itemHeight = item->getHeight();
    
    // Check if item fits at this position
    if (x + itemWidth > m_width || y + itemHeight > m_height) {
        return false;
    }
    
    // Check if all required slots are empty
    for (int dy = 0; dy < itemHeight; dy++) {
        for (int dx = 0; dx < itemWidth; dx++) {
            if (m_grid[y + dy][x + dx]) {
                return false;  // Slot already occupied
            }
        }
    }
    
    // Place the item in all required slots
    for (int dy = 0; dy < itemHeight; dy++) {
        for (int dx = 0; dx < itemWidth; dx++) {
            m_grid[y + dy][x + dx] = item;
        }
    }
    
    m_usedSlots += itemWidth * itemHeight;
    return true;
}

std::shared_ptr<Item> Inventory::getItemAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return nullptr;
    }
    
    return m_grid[y][x];
}

} // namespace d2::game