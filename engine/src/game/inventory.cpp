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

bool Inventory::removeItem(int x, int y) {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return false;
    }
    
    auto item = m_grid[y][x];
    if (!item) {
        return false;  // No item at this position
    }
    
    // Find the top-left corner of the item
    // Items store the same pointer in all occupied slots
    int itemStartX = x;
    int itemStartY = y;
    
    // Search backwards to find the top-left corner
    for (int searchY = y; searchY >= 0 && m_grid[searchY][x] == item; searchY--) {
        itemStartY = searchY;
    }
    for (int searchX = x; searchX >= 0 && m_grid[y][searchX] == item; searchX--) {
        itemStartX = searchX;
    }
    
    // Remove the item from all its slots
    int itemWidth = item->getWidth();
    int itemHeight = item->getHeight();
    
    for (int dy = 0; dy < itemHeight; dy++) {
        for (int dx = 0; dx < itemWidth; dx++) {
            if (itemStartY + dy < m_height && itemStartX + dx < m_width) {
                m_grid[itemStartY + dy][itemStartX + dx] = nullptr;
            }
        }
    }
    
    m_usedSlots -= itemWidth * itemHeight;
    return true;
}

bool Inventory::hasSpaceFor(int itemWidth, int itemHeight) const {
    // Try to find any position where the item fits
    for (int y = 0; y <= m_height - itemHeight; y++) {
        for (int x = 0; x <= m_width - itemWidth; x++) {
            bool canFit = true;
            
            // Check if all required slots are empty
            for (int dy = 0; dy < itemHeight && canFit; dy++) {
                for (int dx = 0; dx < itemWidth && canFit; dx++) {
                    if (m_grid[y + dy][x + dx]) {
                        canFit = false;
                    }
                }
            }
            
            if (canFit) {
                return true;
            }
        }
    }
    
    return false;
}

bool Inventory::contains(std::shared_ptr<Item> item) const {
    if (!item) return false;
    
    // Search through the grid for the item
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            if (m_grid[y][x] == item) {
                return true;
            }
        }
    }
    
    return false;
}

} // namespace d2::game