#include <gtest/gtest.h>
#include <memory>
#include "ui/skill_screen.h"
#include "game/character.h"
#include "game/skill.h"

namespace d2 {

using d2::game::Character;
using d2::game::CharacterClass;
using d2::game::Skill;
using d2::game::SkillType;

class SkillScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        character_ = std::make_unique<Character>(CharacterClass::SORCERESS);
        character_->setLevel(5);
        character_->setStatPoints(20);
        
        // Create some test skills
        fire_bolt_ = std::make_shared<Skill>(SkillType::FIRE, "Fire Bolt");
        fire_ball_ = std::make_shared<Skill>(SkillType::FIRE, "Fire Ball");
        cold_bolt_ = std::make_shared<Skill>(SkillType::COLD, "Ice Bolt");
        
        // Set up skill prerequisites (Fire Ball requires Fire Bolt level 1)
        fire_ball_->setPrerequisite(fire_bolt_.get(), 1);
        
        skill_screen_ = std::make_unique<SkillScreen>(character_.get());
    }
    
    std::unique_ptr<Character> character_;
    std::unique_ptr<SkillScreen> skill_screen_;
    std::shared_ptr<Skill> fire_bolt_;
    std::shared_ptr<Skill> fire_ball_;
    std::shared_ptr<Skill> cold_bolt_;
};

TEST_F(SkillScreenTest, CreateSkillScreen) {
    // SkillScreen should be created successfully with character reference
    EXPECT_EQ(skill_screen_->getCharacterLevel(), 5);
    EXPECT_EQ(skill_screen_->getAvailableSkillPoints(), 4); // Level 5 = 4 skill points
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 0); // Default selection
}

TEST_F(SkillScreenTest, AddSkillsToDisplay) {
    // Should be able to add skills to display list
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->addSkill(fire_ball_);
    skill_screen_->addSkill(cold_bolt_);
    
    EXPECT_EQ(skill_screen_->getSkillCount(), 3);
    EXPECT_EQ(skill_screen_->getSkillAt(0), fire_bolt_);
    EXPECT_EQ(skill_screen_->getSkillAt(1), fire_ball_);
    EXPECT_EQ(skill_screen_->getSkillAt(2), cold_bolt_);
}

TEST_F(SkillScreenTest, SkillSelection) {
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->addSkill(fire_ball_);
    skill_screen_->addSkill(cold_bolt_);
    
    // Should start with first skill selected
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 0);
    EXPECT_EQ(skill_screen_->getSelectedSkill(), fire_bolt_);
    
    // Should be able to navigate to other skills
    skill_screen_->selectSkill(1);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 1);
    EXPECT_EQ(skill_screen_->getSelectedSkill(), fire_ball_);
    
    skill_screen_->selectSkill(2);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 2);
    EXPECT_EQ(skill_screen_->getSelectedSkill(), cold_bolt_);
}

TEST_F(SkillScreenTest, SkillPointAllocation) {
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->selectSkill(0);
    
    // Should have 4 available skill points initially
    EXPECT_EQ(skill_screen_->getAvailableSkillPoints(), 4);
    EXPECT_EQ(fire_bolt_->getLevel(), 0);
    
    // Should be able to allocate skill point
    bool allocated = skill_screen_->allocateSkillPoint();
    EXPECT_TRUE(allocated);
    EXPECT_EQ(fire_bolt_->getLevel(), 1);
    EXPECT_EQ(skill_screen_->getAvailableSkillPoints(), 3);
    
    // Should be able to allocate multiple points
    skill_screen_->allocateSkillPoint();
    skill_screen_->allocateSkillPoint();
    EXPECT_EQ(fire_bolt_->getLevel(), 3);
    EXPECT_EQ(skill_screen_->getAvailableSkillPoints(), 1);
}

TEST_F(SkillScreenTest, PreventInvalidSkillAllocation) {
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->addSkill(fire_ball_);
    
    // Fire Ball requires Fire Bolt level 1, should not be allocatable initially
    skill_screen_->selectSkill(1); // Select Fire Ball
    bool allocated = skill_screen_->allocateSkillPoint();
    EXPECT_FALSE(allocated); // Should fail due to missing prerequisite
    EXPECT_EQ(fire_ball_->getLevel(), 0);
    
    // After leveling Fire Bolt, Fire Ball should be allocatable
    skill_screen_->selectSkill(0); // Select Fire Bolt
    skill_screen_->allocateSkillPoint(); // Level Fire Bolt to 1
    
    skill_screen_->selectSkill(1); // Select Fire Ball again
    allocated = skill_screen_->allocateSkillPoint();
    EXPECT_TRUE(allocated); // Should work now
    EXPECT_EQ(fire_ball_->getLevel(), 1);
}

TEST_F(SkillScreenTest, HandleControllerNavigation) {
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->addSkill(fire_ball_);
    skill_screen_->addSkill(cold_bolt_);
    
    // Should start with first skill selected
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 0);
    
    // Navigate down should move to next skill
    bool handled = skill_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 1);
    
    // Navigate down again
    handled = skill_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 2);
    
    // Navigate down at end should wrap to beginning
    handled = skill_screen_->handleControllerInput(ControllerAction::NAVIGATE_DOWN);
    EXPECT_TRUE(handled);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 0);
    
    // Navigate up should move to previous skill (with wraparound)
    handled = skill_screen_->handleControllerInput(ControllerAction::NAVIGATE_UP);
    EXPECT_TRUE(handled);
    EXPECT_EQ(skill_screen_->getSelectedSkillIndex(), 2);
}

TEST_F(SkillScreenTest, HandleSkillPointAllocationWithController) {
    skill_screen_->addSkill(fire_bolt_);
    skill_screen_->selectSkill(0);
    
    // Action button should allocate skill point
    EXPECT_EQ(fire_bolt_->getLevel(), 0);
    bool handled = skill_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(fire_bolt_->getLevel(), 1);
    
    // Should work multiple times
    handled = skill_screen_->handleControllerInput(ControllerAction::ACTION_BUTTON);
    EXPECT_TRUE(handled);
    EXPECT_EQ(fire_bolt_->getLevel(), 2);
}

} // namespace d2