#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace d2::game {

class Item;

class Inventory {
public:
    Inventory(int width, int height);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTotalSlots() const { return m_width * m_height; }
    int getUsedSlots() const { return m_usedSlots; }
    bool isEmpty() const { return m_usedSlots == 0; }
    bool isFull() const { return m_usedSlots == getTotalSlots(); }
    
    // Item management
    bool addItem(std::shared_ptr<Item> item, int x, int y);
    std::shared_ptr<Item> getItemAt(int x, int y) const;
    bool removeItem(int x, int y);
    
    // Space checking
    bool hasSpaceFor(int itemWidth, int itemHeight) const;
    
private:
    int m_width;
    int m_height;
    int m_usedSlots = 0;
    
    // Grid storage - each slot can hold a pointer to an item
    std::vector<std::vector<std::shared_ptr<Item>>> m_grid;
};

} // namespace d2::game