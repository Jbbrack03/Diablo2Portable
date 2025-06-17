#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace d2::world {

enum class ObjectType {
    DOOR,
    CHEST,
    PORTAL,
    LEVER,
    SHRINE
};

struct LootItem {
    std::string name;
    int quantity;
};

class WorldObject {
public:
    WorldObject(ObjectType type, int x, int y);
    virtual ~WorldObject() = default;
    
    ObjectType getType() const { return m_type; }
    int getPositionX() const { return m_x; }
    int getPositionY() const { return m_y; }
    glm::ivec2 getPosition() const { return {m_x, m_y}; }
    
    virtual bool canInteract() const = 0;
    virtual void interact() = 0;
    
protected:
    ObjectType m_type;
    int m_x, m_y;
};

class Door : public WorldObject {
public:
    Door(int x, int y);
    
    bool isLocked() const { return m_locked; }
    void setLocked(bool locked) { m_locked = locked; }
    bool isOpen() const { return m_open; }
    
    bool canInteract() const override;
    void interact() override;
    
private:
    bool m_locked = false;
    bool m_open = false;
};

class Chest : public WorldObject {
public:
    Chest(int x, int y);
    
    void addItem(const std::string& name, int quantity);
    bool hasItems() const { return !m_items.empty(); }
    int getItemCount() const { return m_items.size(); }
    std::vector<LootItem> open();
    
    bool canInteract() const override;
    void interact() override;
    
private:
    std::vector<LootItem> m_items;
    bool m_opened = false;
};

class Portal : public WorldObject {
public:
    Portal(int x, int y);
    
    void setDestination(const std::string& mapName, int destX, int destY);
    void setActive(bool active) { m_active = active; }
    bool isActive() const { return m_active; }
    std::string getDestinationMap() const { return m_destinationMap; }
    glm::ivec2 getDestination() const { return {m_destX, m_destY}; }
    
    bool canInteract() const override;
    void interact() override;
    
private:
    bool m_active = false;
    std::string m_destinationMap;
    int m_destX = 0, m_destY = 0;
};

class Lever : public WorldObject {
public:
    Lever(int x, int y);
    
    void setTargetObject(int objectId) { m_targetObjectId = objectId; }
    int getTargetObjectId() const { return m_targetObjectId; }
    bool isActivated() const { return m_activated; }
    void activate();
    
    bool canInteract() const override;
    void interact() override;
    
private:
    bool m_activated = false;
    int m_targetObjectId = -1;
};

class Shrine : public WorldObject {
public:
    Shrine(int x, int y);
    
    void setEffectRadius(int radius) { m_effectRadius = radius; }
    int getEffectRadius() const { return m_effectRadius; }
    bool hasBeenActivated() const { return m_activated; }
    void activate() { m_activated = true; }
    
    bool canInteract() const override;
    void interact() override;
    
private:
    int m_effectRadius = 10;
    bool m_activated = false;
};

} // namespace d2::world