#pragma once

#include <string>

namespace d2::game {

enum class ItemType {
    WEAPON,
    ARMOR,
    ACCESSORY,
    CONSUMABLE,
    QUEST
};

class Item {
public:
    Item(const std::string& name, ItemType type);
    
    const std::string& getName() const { return m_name; }
    ItemType getType() const { return m_type; }
    int getRequiredLevel() const { return m_requiredLevel; }
    
private:
    std::string m_name;
    ItemType m_type;
    int m_requiredLevel = 1;
};

} // namespace d2::game