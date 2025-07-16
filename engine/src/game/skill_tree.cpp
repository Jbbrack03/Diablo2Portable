#include "game/skill_tree.h"
#include <memory>

namespace d2::game {

// SkillTab implementation
SkillTab::SkillTab(const std::string& name)
    : m_name(name) {
}

void SkillTab::addSkill(std::unique_ptr<Skill> skill) {
    m_skills.push_back(std::move(skill));
}

Skill* SkillTab::getSkill(int index) const {
    if (index < 0 || index >= static_cast<int>(m_skills.size())) {
        return nullptr;
    }
    return m_skills[index].get();
}

// SkillTree implementation
SkillTree::SkillTree(CharacterClass characterClass)
    : m_characterClass(characterClass) {
    initializeSkillsForClass();
}

CharacterClass SkillTree::getCharacterClass() const {
    return m_characterClass;
}

int SkillTree::getSkillTabCount() const {
    return static_cast<int>(m_skillTabs.size());
}

int SkillTree::getTotalSkillCount() const {
    int totalCount = 0;
    for (const auto& tab : m_skillTabs) {
        totalCount += tab->getSkillCount();
    }
    return totalCount;
}

SkillTab* SkillTree::getSkillTab(int index) const {
    if (index < 0 || index >= static_cast<int>(m_skillTabs.size())) {
        return nullptr;
    }
    return m_skillTabs[index].get();
}

Skill* SkillTree::findSkill(const std::string& skillName) const {
    for (const auto& tab : m_skillTabs) {
        for (int i = 0; i < tab->getSkillCount(); ++i) {
            Skill* skill = tab->getSkill(i);
            if (skill && skill->getName() == skillName) {
                return skill;
            }
        }
    }
    return nullptr;
}

void SkillTree::initializeSkillsForClass() {
    switch (m_characterClass) {
        case CharacterClass::SORCERESS:
            initializeSorceressSkills();
            break;
        case CharacterClass::BARBARIAN:
            initializeBarbarianSkills();
            break;
        case CharacterClass::AMAZON:
            initializeAmazonSkills();
            break;
        case CharacterClass::NECROMANCER:
            initializeNecromancerSkills();
            break;
        case CharacterClass::PALADIN:
            initializePaladinSkills();
            break;
        case CharacterClass::ASSASSIN:
            initializeAssassinSkills();
            break;
        case CharacterClass::DRUID:
            initializeDruidSkills();
            break;
    }
}

void SkillTree::initializeSorceressSkills() {
    // Fire Skills Tab
    auto fireTab = std::make_unique<SkillTab>("Fire Spells");
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Fire Bolt"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Warmth"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Inferno"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Fire Ball"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Blaze"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Fire Wall"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Enchant"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Meteor"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Fire Mastery"));
    fireTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Hydra"));
    m_skillTabs.push_back(std::move(fireTab));
    
    // Cold Skills Tab
    auto coldTab = std::make_unique<SkillTab>("Cold Spells");
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Ice Bolt"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Frozen Armor"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Frost Nova"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Ice Blast"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Shiver Armor"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Glacial Spike"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Blizzard"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Chilling Armor"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Frozen Orb"));
    coldTab->addSkill(std::make_unique<Skill>(SkillType::COLD, "Cold Mastery"));
    m_skillTabs.push_back(std::move(coldTab));
    
    // Lightning Skills Tab
    auto lightningTab = std::make_unique<SkillTab>("Lightning Spells");
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Charged Bolt"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Static Field"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Telekinesis"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Nova"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Lightning"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Chain Lightning"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Teleport"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Thunder Storm"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Energy Shield"));
    lightningTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Lightning Mastery"));
    m_skillTabs.push_back(std::move(lightningTab));
}

void SkillTree::initializeBarbarianSkills() {
    // Combat Skills Tab
    auto combatTab = std::make_unique<SkillTab>("Combat Skills");
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Bash"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Leap"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Double Swing"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Stun"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Double Throw"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Leap Attack"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Concentrate"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Frenzy"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Whirlwind"));
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Berserk"));
    m_skillTabs.push_back(std::move(combatTab));
    
    // Create other tabs with minimal skills for now
    auto masteriesTab = std::make_unique<SkillTab>("Combat Masteries");
    masteriesTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Sword Mastery"));
    m_skillTabs.push_back(std::move(masteriesTab));
    
    auto warcriesTab = std::make_unique<SkillTab>("Warcries");
    warcriesTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Howl"));
    m_skillTabs.push_back(std::move(warcriesTab));
}

void SkillTree::initializeAmazonSkills() {
    // Minimal implementation - just create 3 tabs with at least one skill each
    auto javTab = std::make_unique<SkillTab>("Javelin and Spear Skills");
    javTab->addSkill(std::make_unique<Skill>(SkillType::LIGHTNING, "Jab"));
    m_skillTabs.push_back(std::move(javTab));
    
    auto passiveTab = std::make_unique<SkillTab>("Passive and Magic Skills");
    passiveTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Inner Sight"));
    m_skillTabs.push_back(std::move(passiveTab));
    
    auto bowTab = std::make_unique<SkillTab>("Bow and Crossbow Skills");
    bowTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Magic Arrow"));
    m_skillTabs.push_back(std::move(bowTab));
}

void SkillTree::initializeNecromancerSkills() {
    // Minimal implementation
    auto summoningTab = std::make_unique<SkillTab>("Summoning Spells");
    summoningTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Raise Skeleton"));
    m_skillTabs.push_back(std::move(summoningTab));
    
    auto poisonTab = std::make_unique<SkillTab>("Poison and Bone Spells");
    poisonTab->addSkill(std::make_unique<Skill>(SkillType::POISON, "Teeth"));
    m_skillTabs.push_back(std::move(poisonTab));
    
    auto cursesTab = std::make_unique<SkillTab>("Curses");
    cursesTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Amplify Damage"));
    m_skillTabs.push_back(std::move(cursesTab));
}

void SkillTree::initializePaladinSkills() {
    // Minimal implementation
    auto combatTab = std::make_unique<SkillTab>("Combat Skills");
    combatTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Sacrifice"));
    m_skillTabs.push_back(std::move(combatTab));
    
    auto offensiveTab = std::make_unique<SkillTab>("Offensive Auras");
    offensiveTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Might"));
    m_skillTabs.push_back(std::move(offensiveTab));
    
    auto defensiveTab = std::make_unique<SkillTab>("Defensive Auras");
    defensiveTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Prayer"));
    m_skillTabs.push_back(std::move(defensiveTab));
}

void SkillTree::initializeAssassinSkills() {
    // Minimal implementation
    auto martialTab = std::make_unique<SkillTab>("Martial Arts");
    martialTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Tiger Strike"));
    m_skillTabs.push_back(std::move(martialTab));
    
    auto shadowTab = std::make_unique<SkillTab>("Shadow Disciplines");
    shadowTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Claw Mastery"));
    m_skillTabs.push_back(std::move(shadowTab));
    
    auto trapsTab = std::make_unique<SkillTab>("Traps");
    trapsTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Fire Blast"));
    m_skillTabs.push_back(std::move(trapsTab));
}

void SkillTree::initializeDruidSkills() {
    // Minimal implementation
    auto elementalTab = std::make_unique<SkillTab>("Elemental Skills");
    elementalTab->addSkill(std::make_unique<Skill>(SkillType::FIRE, "Firestorm"));
    m_skillTabs.push_back(std::move(elementalTab));
    
    auto shapeTab = std::make_unique<SkillTab>("Shape Shifting");
    shapeTab->addSkill(std::make_unique<Skill>(SkillType::PHYSICAL, "Werewolf"));
    m_skillTabs.push_back(std::move(shapeTab));
    
    auto summoningTab = std::make_unique<SkillTab>("Summoning Skills");
    summoningTab->addSkill(std::make_unique<Skill>(SkillType::MAGIC, "Raven"));
    m_skillTabs.push_back(std::move(summoningTab));
}

} // namespace d2::game