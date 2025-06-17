#include "ui/skill_screen.h"

namespace d2 {

SkillScreen::SkillScreen(Character* character)
    : character_(character) {
}

int SkillScreen::getCharacterLevel() const {
    return character_->getLevel();
}

int SkillScreen::getAvailableSkillPoints() const {
    // Skill points are level - 1 (no skill point at level 1) minus allocated points
    return character_->getLevel() - 1 - allocated_skill_points_;
}

void SkillScreen::addSkill(std::shared_ptr<Skill> skill) {
    skills_.push_back(skill);
}

size_t SkillScreen::getSkillCount() const {
    return skills_.size();
}

std::shared_ptr<Skill> SkillScreen::getSkillAt(size_t index) const {
    if (index >= skills_.size()) {
        return nullptr;
    }
    return skills_[index];
}

void SkillScreen::selectSkill(size_t index) {
    if (index < skills_.size()) {
        selected_skill_index_ = index;
    }
}

std::shared_ptr<Skill> SkillScreen::getSelectedSkill() const {
    if (selected_skill_index_ >= skills_.size()) {
        return nullptr;
    }
    return skills_[selected_skill_index_];
}

bool SkillScreen::allocateSkillPoint() {
    if (!canAllocateToSelectedSkill()) {
        return false;
    }
    
    auto selected_skill = getSelectedSkill();
    if (!selected_skill) {
        return false;
    }
    
    // Check if we have available skill points
    if (getAvailableSkillPoints() <= 0) {
        return false;
    }
    
    // Add skill point to the selected skill
    bool success = selected_skill->addSkillPoint();
    if (success) {
        // Track allocated skill points
        allocated_skill_points_++;
    }
    
    return success;
}

bool SkillScreen::handleControllerInput(ControllerAction action) {
    switch (action) {
        case ControllerAction::NAVIGATE_UP:
            navigateToPreviousSkill();
            return true;
            
        case ControllerAction::NAVIGATE_DOWN:
            navigateToNextSkill();
            return true;
            
        case ControllerAction::ACTION_BUTTON:
            return allocateSkillPoint();
            
        case ControllerAction::CANCEL_BUTTON:
            // Handle cancel (could close screen, etc.)
            return true;
            
        default:
            return false;
    }
}

void SkillScreen::navigateToNextSkill() {
    if (skills_.empty()) {
        return;
    }
    
    selected_skill_index_ = (selected_skill_index_ + 1) % skills_.size();
}

void SkillScreen::navigateToPreviousSkill() {
    if (skills_.empty()) {
        return;
    }
    
    if (selected_skill_index_ == 0) {
        selected_skill_index_ = skills_.size() - 1;
    } else {
        selected_skill_index_--;
    }
}

bool SkillScreen::canAllocateToSelectedSkill() const {
    auto selected_skill = getSelectedSkill();
    if (!selected_skill) {
        return false;
    }
    
    // Check if skill can accept more points
    if (!selected_skill->canAddSkillPoint()) {
        return false;
    }
    
    // For now, we'll assume prerequisites are handled by the Skill class
    // The Skill::addSkillPoint() method should check prerequisites
    return true;
}

} // namespace d2