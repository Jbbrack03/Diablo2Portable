#include <gtest/gtest.h>
#include "game/character_inventory.h"
#include "game/item.h"

using namespace d2::game;

// Helper functions to create test items
std::shared_ptr<Item> createTestSword() {
    auto sword = std::make_shared<Item>("Test Sword", ItemType::WEAPON);
    sword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    sword->setRequiredLevel(1);
    sword->setRequiredStrength(10);
    sword->setRequiredDexterity(10);
    return sword;
}

std::shared_ptr<Item> createTestArmor() {
    auto armor = std::make_shared<Item>("Test Armor", ItemType::ARMOR);
    armor->setEquipmentSlot(EquipmentSlot::TORSO);
    armor->setRequiredLevel(5);
    armor->setRequiredStrength(25);
    armor->setRequiredDexterity(15);
    return armor;
}

std::shared_ptr<Item> createTwoHandedWeapon() {
    auto weapon = std::make_shared<Item>("Two-Handed Sword", ItemType::WEAPON);
    weapon->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    weapon->setTwoHanded(true);
    weapon->setRequiredLevel(10);
    weapon->setRequiredStrength(40);
    weapon->setRequiredDexterity(20);
    return weapon;
}

std::shared_ptr<Item> createShield() {
    auto shield = std::make_shared<Item>("Test Shield", ItemType::ARMOR);
    shield->setEquipmentSlot(EquipmentSlot::OFF_HAND);
    shield->setRequiredLevel(5);
    shield->setRequiredStrength(20);
    shield->setRequiredDexterity(15);
    return shield;
}

class CharacterInventoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        inventory = std::make_unique<CharacterInventory>();
        inventory->setCharacterLevel(20);
        inventory->setCharacterStrength(50);
        inventory->setCharacterDexterity(30);
    }
    
    std::unique_ptr<CharacterInventory> inventory;
};

// Test basic item equipment functionality
TEST_F(CharacterInventoryTest, EquipBasicItem) {
    auto sword = createTestSword();
    
    EXPECT_TRUE(inventory->equipItem(sword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword);
}

// Test equipment slot validation
TEST_F(CharacterInventoryTest, GetEmptyEquipmentSlot) {
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::TORSO), nullptr);
}

// Test equipment validation with insufficient level requirement
TEST_F(CharacterInventoryTest, EquipItemWithInsufficientLevelFails) {
    auto highLevelSword = std::make_shared<Item>("High Level Sword", ItemType::WEAPON);
    highLevelSword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    highLevelSword->setRequiredLevel(50);  // Character is only level 20
    highLevelSword->setRequiredStrength(10);
    highLevelSword->setRequiredDexterity(10);
    
    EXPECT_FALSE(inventory->equipItemWithValidation(highLevelSword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
}

// Test equipment validation with insufficient strength requirement
TEST_F(CharacterInventoryTest, EquipItemWithInsufficientStrengthFails) {
    auto heavySword = std::make_shared<Item>("Heavy Sword", ItemType::WEAPON);
    heavySword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    heavySword->setRequiredLevel(1);
    heavySword->setRequiredStrength(100);  // Character only has 50 strength
    heavySword->setRequiredDexterity(10);
    
    EXPECT_FALSE(inventory->equipItemWithValidation(heavySword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
}

// Test equipment validation with insufficient dexterity requirement  
TEST_F(CharacterInventoryTest, EquipItemWithInsufficientDexterityFails) {
    auto precisionSword = std::make_shared<Item>("Precision Sword", ItemType::WEAPON);
    precisionSword->setEquipmentSlot(EquipmentSlot::MAIN_HAND);
    precisionSword->setRequiredLevel(1);
    precisionSword->setRequiredStrength(10);
    precisionSword->setRequiredDexterity(100);  // Character only has 30 dexterity
    
    EXPECT_FALSE(inventory->equipItemWithValidation(precisionSword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
}

// Test successful equipment validation
TEST_F(CharacterInventoryTest, EquipItemWithValidationSucceeds) {
    auto sword = createTestSword();
    
    EXPECT_TRUE(inventory->equipItemWithValidation(sword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword);
}

// Test equipment swapping
TEST_F(CharacterInventoryTest, EquipItemSwapsExistingItem) {
    auto sword1 = createTestSword();
    auto sword2 = createTestSword();
    
    // Equip first sword
    EXPECT_TRUE(inventory->equipItem(sword1));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword1);
    
    // Equip second sword - should swap and move first to backpack
    EXPECT_TRUE(inventory->equipItem(sword2));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword2);
    
    // First sword should be in backpack
    auto& backpack = inventory->getBackpack();
    bool foundInBackpack = false;
    for (int y = 0; y < backpack.getHeight(); y++) {
        for (int x = 0; x < backpack.getWidth(); x++) {
            if (backpack.getItemAt(x, y) == sword1) {
                foundInBackpack = true;
                break;
            }
        }
    }
    EXPECT_TRUE(foundInBackpack);
}

// Test unequipping item
TEST_F(CharacterInventoryTest, UnequipItem) {
    auto sword = createTestSword();
    
    // Equip the sword
    EXPECT_TRUE(inventory->equipItem(sword));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword);
    
    // Unequip the sword
    EXPECT_TRUE(inventory->unequipItem(EquipmentSlot::MAIN_HAND));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
    
    // Sword should be in backpack
    auto& backpack = inventory->getBackpack();
    bool foundInBackpack = false;
    for (int y = 0; y < backpack.getHeight(); y++) {
        for (int x = 0; x < backpack.getWidth(); x++) {
            if (backpack.getItemAt(x, y) == sword) {
                foundInBackpack = true;
                break;
            }
        }
    }
    EXPECT_TRUE(foundInBackpack);
}

// Test unequipping from empty slot
TEST_F(CharacterInventoryTest, UnequipFromEmptySlotFails) {
    EXPECT_FALSE(inventory->unequipItem(EquipmentSlot::MAIN_HAND));
}

// Test two-handed weapon restriction
TEST_F(CharacterInventoryTest, TwoHandedWeaponPreventsOffHand) {
    auto twoHander = createTwoHandedWeapon();
    auto shield = createShield();
    
    // Equip two-handed weapon first
    EXPECT_TRUE(inventory->equipItemWithValidation(twoHander));
    
    // Try to equip shield - should fail
    EXPECT_FALSE(inventory->equipItemWithValidation(shield));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::OFF_HAND), nullptr);
}

// Test off-hand prevents two-handed weapon
TEST_F(CharacterInventoryTest, OffHandPreventersTwoHandedWeapon) {
    auto shield = createShield();
    auto twoHander = createTwoHandedWeapon();
    
    // Equip shield first
    EXPECT_TRUE(inventory->equipItemWithValidation(shield));
    
    // Try to equip two-handed weapon - should fail
    EXPECT_FALSE(inventory->equipItemWithValidation(twoHander));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), nullptr);
}

// Test isTwoHandedEquipped functionality
TEST_F(CharacterInventoryTest, DetectTwoHandedWeaponEquipped) {
    auto twoHander = createTwoHandedWeapon();
    
    EXPECT_FALSE(inventory->isTwoHandedEquipped());
    
    inventory->equipItem(twoHander);
    EXPECT_TRUE(inventory->isTwoHandedEquipped());
}

// Test force equip to specific slot
TEST_F(CharacterInventoryTest, ForceEquipToCorrectSlot) {
    auto sword = createTestSword();
    
    EXPECT_TRUE(inventory->forceEquipToSlot(sword, EquipmentSlot::MAIN_HAND));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::MAIN_HAND), sword);
}

// Test force equip to wrong slot fails
TEST_F(CharacterInventoryTest, ForceEquipToWrongSlotFails) {
    auto sword = createTestSword();  // Main hand weapon
    
    // Try to force equip to wrong slot
    EXPECT_FALSE(inventory->forceEquipToSlot(sword, EquipmentSlot::TORSO));
    EXPECT_EQ(inventory->getEquippedItem(EquipmentSlot::TORSO), nullptr);
}

// Test character stats getters/setters
TEST_F(CharacterInventoryTest, CharacterStatsManagement) {
    EXPECT_EQ(inventory->getCharacterLevel(), 20);
    EXPECT_EQ(inventory->getCharacterStrength(), 50);
    EXPECT_EQ(inventory->getCharacterDexterity(), 30);
    
    inventory->setCharacterLevel(25);
    inventory->setCharacterStrength(60);
    inventory->setCharacterDexterity(40);
    
    EXPECT_EQ(inventory->getCharacterLevel(), 25);
    EXPECT_EQ(inventory->getCharacterStrength(), 60);
    EXPECT_EQ(inventory->getCharacterDexterity(), 40);
}

// Test equipping null item fails
TEST_F(CharacterInventoryTest, EquipNullItemFails) {
    EXPECT_FALSE(inventory->equipItem(nullptr));
    EXPECT_FALSE(inventory->equipItemWithValidation(nullptr));
    EXPECT_FALSE(inventory->forceEquipToSlot(nullptr, EquipmentSlot::MAIN_HAND));
}