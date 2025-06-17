#include "world/world_object.h"

namespace d2::world {

WorldObject::WorldObject(ObjectType type, int x, int y)
    : m_type(type), m_x(x), m_y(y)
{
}

// Door implementation
Door::Door(int x, int y) : WorldObject(ObjectType::DOOR, x, y) {
}

bool Door::canInteract() const {
    return !m_open;  // Can interact if door is closed
}

void Door::interact() {
    if (!m_locked) {
        m_open = !m_open;  // Toggle door state
    }
}

// Chest implementation
Chest::Chest(int x, int y) : WorldObject(ObjectType::CHEST, x, y) {
}

void Chest::addItem(const std::string& name, int quantity) {
    LootItem item;
    item.name = name;
    item.quantity = quantity;
    m_items.push_back(item);
}

std::vector<LootItem> Chest::open() {
    std::vector<LootItem> items = m_items;
    m_items.clear();
    m_opened = true;
    return items;
}

bool Chest::canInteract() const {
    return !m_opened && hasItems();
}

void Chest::interact() {
    if (canInteract()) {
        open();
    }
}

// Portal implementation
Portal::Portal(int x, int y) : WorldObject(ObjectType::PORTAL, x, y) {
}

void Portal::setDestination(const std::string& mapName, int destX, int destY) {
    m_destinationMap = mapName;
    m_destX = destX;
    m_destY = destY;
}

bool Portal::canInteract() const {
    return m_active && !m_destinationMap.empty();
}

void Portal::interact() {
    // Portal interaction would typically trigger level transition
    // For now, just ensure it's active
}

// Lever implementation
Lever::Lever(int x, int y) : WorldObject(ObjectType::LEVER, x, y) {
}

void Lever::activate() {
    m_activated = true;
}

bool Lever::canInteract() const {
    return !m_activated;
}

void Lever::interact() {
    activate();
}

// Shrine implementation
Shrine::Shrine(int x, int y) : WorldObject(ObjectType::SHRINE, x, y) {
}

bool Shrine::canInteract() const {
    return !m_activated;
}

void Shrine::interact() {
    activate();
}

} // namespace d2::world