#include "game/inventory.h"
#include "game/item.h"
#include <algorithm>

namespace d2::game {

Inventory::Inventory(int width, int height) 
    : m_width(width), m_height(height) {
    m_grid.resize(height);
    for (auto& row : m_grid) {
        row.resize(width);
    }
}

bool Inventory::addItem(std::shared_ptr<Item> item, int x, int y) {
    if (!item) return false;
    
    int itemWidth = item->getWidth();
    int itemHeight = item->getHeight();
    
    // Check bounds
    if (x < 0 || y < 0 || x + itemWidth > m_width || y + itemHeight > m_height) {
        return false;
    }
    
    // Check if space is available
    for (int dy = 0; dy < itemHeight; dy++) {
        for (int dx = 0; dx < itemWidth; dx++) {
            if (m_grid[y + dy][x + dx] != nullptr) {
                return false;  // Space occupied
            }
        }
    }
    
    // Place item in all occupied slots
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
    auto item = getItemAt(x, y);
    if (!item) return false;
    
    // Find the top-left corner of the item
    int topX = x, topY = y;
    for (int dy = y; dy >= 0; dy--) {
        for (int dx = x; dx >= 0; dx--) {
            if (m_grid[dy][dx] == item) {
                topX = dx;
                topY = dy;
            }
        }
    }
    
    // Remove item from all occupied slots
    int itemWidth = item->getWidth();
    int itemHeight = item->getHeight();
    
    for (int dy = 0; dy < itemHeight; dy++) {
        for (int dx = 0; dx < itemWidth; dx++) {
            if (topY + dy < m_height && topX + dx < m_width) {
                m_grid[topY + dy][topX + dx] = nullptr;
            }
        }
    }
    
    m_usedSlots -= itemWidth * itemHeight;
    return true;
}

bool Inventory::hasSpaceFor(int itemWidth, int itemHeight) const {
    for (int y = 0; y <= m_height - itemHeight; y++) {
        for (int x = 0; x <= m_width - itemWidth; x++) {
            bool canFit = true;
            
            for (int dy = 0; dy < itemHeight && canFit; dy++) {
                for (int dx = 0; dx < itemWidth && canFit; dx++) {
                    if (m_grid[y + dy][x + dx] != nullptr) {
                        canFit = false;
                    }
                }
            }
            
            if (canFit) return true;
        }
    }
    return false;
}

bool Inventory::contains(std::shared_ptr<Item> item) const {
    for (const auto& row : m_grid) {
        for (const auto& slot : row) {
            if (slot == item) {
                return true;
            }
        }
    }
    return false;
}

bool Inventory::addStackableItem(std::shared_ptr<Item> item) {
    if (!item || !item->isStackable()) {
        return false;
    }
    
    // First, try to find an existing stack of the same item
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            auto existingItem = m_grid[y][x];
            if (existingItem && existingItem->canStackWith(*item)) {
                int currentQuantity = existingItem->getQuantity();
                int maxStack = existingItem->getMaxStackSize();
                int spaceAvailable = maxStack - currentQuantity;
                
                if (spaceAvailable > 0) {
                    int toAdd = std::min(spaceAvailable, item->getQuantity());
                    existingItem->setQuantity(currentQuantity + toAdd);
                    item->setQuantity(item->getQuantity() - toAdd);
                    
                    if (item->getQuantity() == 0) {
                        return true;  // All items stacked
                    }
                }
            }
        }
    }
    
    // If we still have items left, find empty space for new stack
    if (item->getQuantity() > 0) {
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                if (m_grid[y][x] == nullptr) {
                    // Create new stack with remaining quantity
                    auto newStack = std::make_shared<Item>(item->getName(), item->getType());
                    newStack->setSize(item->getWidth(), item->getHeight());
                    newStack->setStackable(true);
                    newStack->setMaxStackSize(item->getMaxStackSize());
                    newStack->setQuantity(item->getQuantity());
                    
                    if (addItem(newStack, x, y)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

std::shared_ptr<Item> Inventory::splitStack(int x, int y, int quantity) {
    auto item = getItemAt(x, y);
    if (!item || !item->isStackable() || quantity <= 0) {
        return nullptr;
    }
    
    int currentQuantity = item->getQuantity();
    if (quantity >= currentQuantity) {
        return nullptr;  // Can't split entire stack or more
    }
    
    // Create new item with split quantity
    auto splitItem = std::make_shared<Item>(item->getName(), item->getType());
    splitItem->setSize(item->getWidth(), item->getHeight());
    splitItem->setStackable(true);
    splitItem->setMaxStackSize(item->getMaxStackSize());
    splitItem->setQuantity(quantity);
    
    // Reduce original stack
    item->setQuantity(currentQuantity - quantity);
    
    return splitItem;
}

bool Inventory::mergeStacks(int sourceX, int sourceY, int targetX, int targetY) {
    auto sourceItem = getItemAt(sourceX, sourceY);
    auto targetItem = getItemAt(targetX, targetY);
    
    if (!sourceItem || !targetItem) {
        return false;
    }
    
    if (!sourceItem->canStackWith(*targetItem)) {
        return false;
    }
    
    int sourceQuantity = sourceItem->getQuantity();
    int targetQuantity = targetItem->getQuantity();
    int maxStack = targetItem->getMaxStackSize();
    int spaceAvailable = maxStack - targetQuantity;
    
    if (spaceAvailable <= 0) {
        return false;  // Target stack is full
    }
    
    int toTransfer = std::min(spaceAvailable, sourceQuantity);
    targetItem->setQuantity(targetQuantity + toTransfer);
    sourceItem->setQuantity(sourceQuantity - toTransfer);
    
    // If source is empty, remove it
    if (sourceItem->getQuantity() == 0) {
        removeItem(sourceX, sourceY);
    }
    
    return true;
}

} // namespace d2::game