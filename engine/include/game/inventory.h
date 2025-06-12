#pragma once

#include <cstdint>

namespace d2::game {

class Inventory {
public:
    Inventory(int width, int height);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTotalSlots() const { return m_width * m_height; }
    int getUsedSlots() const { return m_usedSlots; }
    bool isEmpty() const { return m_usedSlots == 0; }
    
private:
    int m_width;
    int m_height;
    int m_usedSlots = 0;
};

} // namespace d2::game