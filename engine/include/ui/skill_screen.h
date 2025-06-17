#pragma once

#include "ui/ui_element.h"
#include "ui/ui_manager.h"
#include "game/character.h"
#include "game/skill.h"
#include <memory>
#include <vector>

namespace d2 {

using d2::game::Character;
using d2::game::Skill;

class SkillScreen : public UIElement {
public:
    explicit SkillScreen(Character* character);
    virtual ~SkillScreen() = default;
    
    // Character information
    int getCharacterLevel() const;
    int getAvailableSkillPoints() const;
    
    // Skill management
    void addSkill(std::shared_ptr<Skill> skill);
    size_t getSkillCount() const;
    std::shared_ptr<Skill> getSkillAt(size_t index) const;
    
    // Skill selection
    void selectSkill(size_t index);
    size_t getSelectedSkillIndex() const { return selected_skill_index_; }
    std::shared_ptr<Skill> getSelectedSkill() const;
    
    // Skill point allocation
    bool allocateSkillPoint();
    
    // Controller input
    bool handleControllerInput(ControllerAction action);
    
private:
    Character* character_;
    std::vector<std::shared_ptr<Skill>> skills_;
    size_t selected_skill_index_{0};
    int allocated_skill_points_{0};  // Track allocated points for this session
    
    // Helper methods
    void navigateToNextSkill();
    void navigateToPreviousSkill();
    bool canAllocateToSelectedSkill() const;
};

} // namespace d2