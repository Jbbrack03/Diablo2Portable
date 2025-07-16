#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <glm/vec2.hpp>

namespace d2::game {

enum class NPCType {
    VENDOR,
    QUEST_GIVER,
    HIRABLE,
    NEUTRAL,
    MONSTER
};

enum class NPCState {
    IDLE,
    TALKING,
    WALKING,
    ATTACKING,
    DEAD
};

class NPC {
public:
    NPC(const std::string& name, NPCType type);
    virtual ~NPC() = default;

    // Basic properties
    const std::string& getName() const { return name_; }
    NPCType getType() const { return type_; }
    NPCState getState() const { return state_; }
    
    // Position and movement
    glm::vec2 getPosition() const { return position_; }
    void setPosition(const glm::vec2& pos) { position_ = pos; }
    
    // Interaction
    virtual bool canInteract() const;
    virtual void interact();
    
    // Animation and rendering
    const std::string& getCurrentAnimation() const { return current_animation_; }
    void setAnimation(const std::string& animation);
    
    // Health and combat
    int getHealth() const { return health_; }
    int getMaxHealth() const { return max_health_; }
    void setHealth(int health) { health_ = health; }
    
    // Dialog system
    const std::vector<std::string>& getDialogOptions() const { return dialog_options_; }
    void addDialogOption(const std::string& option);
    
    // Update
    virtual void update(float deltaTime);

protected:
    std::string name_;
    NPCType type_;
    NPCState state_;
    glm::vec2 position_;
    std::string current_animation_;
    int health_;
    int max_health_;
    std::vector<std::string> dialog_options_;
    
    void setState(NPCState state) { state_ = state; }
};

class Vendor : public NPC {
public:
    Vendor(const std::string& name);
    
    // Vendor-specific functionality
    void addItem(uint32_t item_id, int quantity, int price);
    void removeItem(uint32_t item_id);
    bool hasItem(uint32_t item_id) const;
    int getItemPrice(uint32_t item_id) const;
    
    bool canInteract() const override;
    void interact() override;
    
private:
    struct VendorItem {
        uint32_t item_id;
        int quantity;
        int price;
    };
    
    std::vector<VendorItem> inventory_;
};

class QuestGiver : public NPC {
public:
    QuestGiver(const std::string& name);
    
    // Quest-specific functionality
    void addQuest(uint32_t quest_id);
    bool hasQuest(uint32_t quest_id) const;
    void completeQuest(uint32_t quest_id);
    
    bool canInteract() const override;
    void interact() override;
    
private:
    std::vector<uint32_t> available_quests_;
    std::vector<uint32_t> completed_quests_;
};

} // namespace d2::game