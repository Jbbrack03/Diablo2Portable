#include "ui/character_screen.h"

namespace d2 {

CharacterScreen::CharacterScreen(Character* character)
    : character_(character) {
}

CharacterClass CharacterScreen::getCharacterClass() const {
    return character_->getCharacterClass();
}

int CharacterScreen::getCharacterLevel() const {
    return character_->getLevel();
}

int CharacterScreen::getDisplayedLevel() const {
    return character_->getLevel();
}

int CharacterScreen::getAvailableStatPoints() const {
    return character_->getStatPoints();
}

int CharacterScreen::getDisplayedStrength() const {
    return character_->getStrength();
}

int CharacterScreen::getDisplayedDexterity() const {
    return character_->getDexterity();
}

int CharacterScreen::getDisplayedVitality() const {
    return character_->getVitality();
}

int CharacterScreen::getDisplayedEnergy() const {
    return character_->getEnergy();
}

int CharacterScreen::getDisplayedLife() const {
    // Calculate life based on vitality and character class
    int baseLife = 50; // Base life for all characters
    int vitalityBonus = character_->getVitality() * 4; // 4 life per vitality point
    
    // Character class bonuses
    int classBonus = 0;
    if (character_->getCharacterClass() == CharacterClass::BARBARIAN) {
        classBonus = 50; // Barbarians get extra life
    }
    
    return baseLife + vitalityBonus + classBonus;
}

int CharacterScreen::getDisplayedMana() const {
    // Calculate mana based on energy and character class
    int baseMana = 20; // Base mana for all characters
    int energyBonus = character_->getEnergy() * 2; // 2 mana per energy point
    
    // Character class bonuses
    int classBonus = 0;
    if (character_->getCharacterClass() == CharacterClass::SORCERESS) {
        classBonus = 30; // Sorceresses get extra mana
    }
    
    return baseMana + energyBonus + classBonus;
}

int CharacterScreen::getDisplayedStamina() const {
    // Calculate stamina based on vitality and dexterity
    int baseStamina = 100;
    int vitalityBonus = character_->getVitality() * 2;
    int dexterityBonus = character_->getDexterity() * 1;
    
    return baseStamina + vitalityBonus + dexterityBonus;
}

void CharacterScreen::selectStat(StatType stat) {
    selected_stat_ = stat;
}

bool CharacterScreen::allocateStatPoint() {
    if (character_->getStatPoints() <= 0) {
        return false;
    }
    
    // Allocate one point to selected stat
    character_->addStatPoint(selected_stat_, 1);
    
    // Reduce available stat points
    character_->setStatPoints(character_->getStatPoints() - 1);
    
    return true;
}

bool CharacterScreen::handleControllerInput(ControllerAction action) {
    switch (action) {
        case ControllerAction::NAVIGATE_UP:
            navigateToPreviousStat();
            break;
            
        case ControllerAction::NAVIGATE_DOWN:
            navigateToNextStat();
            break;
            
        case ControllerAction::ACTION_BUTTON:
            return allocateStatPoint();
            
        case ControllerAction::NAVIGATE_LEFT:
        case ControllerAction::NAVIGATE_RIGHT:
        case ControllerAction::CANCEL_BUTTON:
            // Handle other actions if needed
            break;
    }
    
    return true;
}

void CharacterScreen::navigateToNextStat() {
    switch (selected_stat_) {
        case StatType::STRENGTH:
            selected_stat_ = StatType::DEXTERITY;
            break;
        case StatType::DEXTERITY:
            selected_stat_ = StatType::VITALITY;
            break;
        case StatType::VITALITY:
            selected_stat_ = StatType::ENERGY;
            break;
        case StatType::ENERGY:
            selected_stat_ = StatType::STRENGTH; // Wrap around
            break;
        case StatType::DAMAGE:
            // DAMAGE is not selectable in character screen
            selected_stat_ = StatType::STRENGTH;
            break;
    }
}

void CharacterScreen::navigateToPreviousStat() {
    switch (selected_stat_) {
        case StatType::STRENGTH:
            selected_stat_ = StatType::ENERGY; // Wrap around
            break;
        case StatType::DEXTERITY:
            selected_stat_ = StatType::STRENGTH;
            break;
        case StatType::VITALITY:
            selected_stat_ = StatType::DEXTERITY;
            break;
        case StatType::ENERGY:
            selected_stat_ = StatType::VITALITY;
            break;
        case StatType::DAMAGE:
            // DAMAGE is not selectable in character screen
            selected_stat_ = StatType::ENERGY;
            break;
    }
}

} // namespace d2