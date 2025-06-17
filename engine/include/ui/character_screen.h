#pragma once

#include "ui/ui_element.h"
#include "ui/ui_manager.h"
#include "game/character.h"
#include <memory>

namespace d2 {

using d2::game::Character;
using d2::game::CharacterClass;
using d2::game::StatType;

class CharacterScreen : public UIElement {
public:
    explicit CharacterScreen(Character* character);
    virtual ~CharacterScreen() = default;
    
    // Character information display
    CharacterClass getCharacterClass() const;
    int getCharacterLevel() const;
    int getDisplayedLevel() const;
    int getAvailableStatPoints() const;
    
    // Stat display
    int getDisplayedStrength() const;
    int getDisplayedDexterity() const;
    int getDisplayedVitality() const;
    int getDisplayedEnergy() const;
    
    // Derived stats display
    int getDisplayedLife() const;
    int getDisplayedMana() const;
    int getDisplayedStamina() const;
    
    // Stat selection and allocation
    void selectStat(StatType stat);
    StatType getSelectedStat() const { return selected_stat_; }
    bool allocateStatPoint();
    
    // Controller input
    bool handleControllerInput(ControllerAction action);
    
private:
    Character* character_;
    StatType selected_stat_{StatType::STRENGTH};
    
    // Helper methods
    void navigateToNextStat();
    void navigateToPreviousStat();
};

} // namespace d2