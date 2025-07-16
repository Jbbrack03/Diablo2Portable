#include "game/item_database.h"
#include "utils/data_table_parser.h"
#include "core/asset_manager.h"
#include <fstream>

namespace d2::game {

bool ItemDatabase::loadFromAssetManager(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                                       d2::utils::DataTableParser* parser) {
    if (!assetManager || !parser) {
        return false;
    }
    
    bool success = true;
    
    // Load armor data
    if (assetManager->hasFile("data/armor.txt")) {
        success &= loadArmorData(assetManager, parser);
    }
    
    // Load weapon data
    if (assetManager->hasFile("data/weapons.txt")) {
        success &= loadWeaponData(assetManager, parser);
    }
    
    // Load misc item data
    if (assetManager->hasFile("data/misc.txt")) {
        success &= loadMiscData(assetManager, parser);
    }
    
    return success;
}

bool ItemDatabase::hasItem(const std::string& itemName) const {
    return items_.find(itemName) != items_.end();
}

const Item& ItemDatabase::getItem(const std::string& itemName) const {
    auto it = items_.find(itemName);
    if (it != items_.end()) {
        return it->second;
    }
    return defaultItem_;
}

bool ItemDatabase::loadArmorData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                                d2::utils::DataTableParser* parser) {
    auto fileData = assetManager->loadFileData("data/armor.txt");
    if (fileData.empty()) {
        return false;
    }
    
    std::string dataStr(fileData.begin(), fileData.end());
    auto table = parser->parseExcel(dataStr);
    
    for (const auto& row : table.getAllRows()) {
        auto nameIt = row.find("name");
        if (nameIt == row.end()) continue;
        
        Item armor(nameIt->second, ItemType::ARMOR);
        
        // Set armor properties
        armor.setDefense(table.getIntValue(row, "ac"));
        armor.setRequiredStrength(table.getIntValue(row, "reqstr"));
        armor.setRequiredLevel(table.getIntValue(row, "level"));
        
        // Set inventory size
        int width = table.getIntValue(row, "invwidth");
        int height = table.getIntValue(row, "invheight");
        if (width > 0 && height > 0) {
            armor.setSize(width, height);
        }
        
        // Determine equipment slot based on type
        auto typeIt = row.find("type");
        if (typeIt != row.end()) {
            if (typeIt->second == "body") {
                armor.setEquipmentSlot(EquipmentSlot::TORSO);
            } else if (typeIt->second == "helm") {
                armor.setEquipmentSlot(EquipmentSlot::HEAD);
            } else if (typeIt->second == "glov") {
                armor.setEquipmentSlot(EquipmentSlot::HANDS);
            } else if (typeIt->second == "boot") {
                armor.setEquipmentSlot(EquipmentSlot::FEET);
            } else if (typeIt->second == "belt") {
                armor.setEquipmentSlot(EquipmentSlot::BELT);
            }
        }
        
        items_.insert({nameIt->second, armor});
    }
    
    return true;
}

bool ItemDatabase::loadWeaponData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                                 d2::utils::DataTableParser* parser) {
    auto fileData = assetManager->loadFileData("data/weapons.txt");
    if (fileData.empty()) {
        return false;
    }
    
    std::string dataStr(fileData.begin(), fileData.end());
    auto table = parser->parseExcel(dataStr);
    
    for (const auto& row : table.getAllRows()) {
        auto nameIt = row.find("name");
        if (nameIt == row.end()) continue;
        
        Item weapon(nameIt->second, ItemType::WEAPON);
        
        // Set weapon properties
        weapon.setDamage(table.getIntValue(row, "mindamage"), 
                        table.getIntValue(row, "maxdamage"));
        weapon.setRequiredStrength(table.getIntValue(row, "reqstr"));
        weapon.setRequiredLevel(table.getIntValue(row, "level"));
        
        // Set inventory size
        int width = table.getIntValue(row, "invwidth");
        int height = table.getIntValue(row, "invheight");
        if (width > 0 && height > 0) {
            weapon.setSize(width, height);
        }
        
        // Check if two-handed
        auto typeIt = row.find("type");
        if (typeIt != row.end()) {
            if (typeIt->second.find("2h") != std::string::npos) {
                weapon.setTwoHanded(true);
            }
        }
        
        weapon.setEquipmentSlot(EquipmentSlot::MAIN_HAND);
        items_.insert({nameIt->second, weapon});
    }
    
    return true;
}

bool ItemDatabase::loadMiscData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                               d2::utils::DataTableParser* parser) {
    auto fileData = assetManager->loadFileData("data/misc.txt");
    if (fileData.empty()) {
        return false;
    }
    
    std::string dataStr(fileData.begin(), fileData.end());
    auto table = parser->parseExcel(dataStr);
    
    for (const auto& row : table.getAllRows()) {
        auto nameIt = row.find("name");
        if (nameIt == row.end()) continue;
        
        // Determine item type from data
        ItemType itemType = ItemType::CONSUMABLE;
        auto typeIt = row.find("type");
        if (typeIt != row.end()) {
            if (typeIt->second == "gold") {
                itemType = ItemType::GOLD;
            } else if (typeIt->second == "quest") {
                itemType = ItemType::QUEST;
            }
        }
        
        Item misc(nameIt->second, itemType);
        
        // Set misc properties
        misc.setRequiredLevel(table.getIntValue(row, "level"));
        
        // Check if stackable
        if (table.getIntValue(row, "stackable") > 0) {
            misc.setStackable(true);
            misc.setMaxStackSize(table.getIntValue(row, "maxstack"));
        }
        
        // For gold items, set the amount
        if (itemType == ItemType::GOLD) {
            misc.setGoldAmount(table.getIntValue(row, "cost"));
        }
        
        items_.insert({nameIt->second, misc});
    }
    
    return true;
}

} // namespace d2::game