#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "game/item.h"

namespace d2portable::core {
    class AssetManager;
}

namespace d2::utils {
    class DataTableParser;
}

namespace d2::game {

/**
 * Database of all item definitions loaded from data tables
 * 
 * This class loads and manages item data from Diablo II's
 * data tables (armor.txt, weapons.txt, misc.txt, etc.)
 */
class ItemDatabase {
public:
    ItemDatabase() : defaultItem_("Unknown Item", ItemType::CONSUMABLE) {}
    ~ItemDatabase() = default;
    
    /**
     * Load item data from asset manager using data table parser
     * @param assetManager The asset manager to load files from
     * @param parser The data table parser to parse the files
     * @return true if loading successful, false otherwise
     */
    bool loadFromAssetManager(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                             d2::utils::DataTableParser* parser);
    
    /**
     * Check if an item exists in the database
     * @param itemName The name of the item
     * @return true if item exists, false otherwise
     */
    bool hasItem(const std::string& itemName) const;
    
    /**
     * Get an item by name
     * @param itemName The name of the item
     * @return The item, or a default item if not found
     */
    const Item& getItem(const std::string& itemName) const;
    
    /**
     * Get all items in the database
     * @return Map of item names to items
     */
    const std::unordered_map<std::string, Item>& getAllItems() const { return items_; }
    
private:
    /**
     * Load armor data from armor.txt
     */
    bool loadArmorData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                      d2::utils::DataTableParser* parser);
    
    /**
     * Load weapon data from weapons.txt
     */
    bool loadWeaponData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                       d2::utils::DataTableParser* parser);
    
    /**
     * Load misc item data from misc.txt
     */
    bool loadMiscData(std::shared_ptr<d2portable::core::AssetManager> assetManager,
                     d2::utils::DataTableParser* parser);
    
    std::unordered_map<std::string, Item> items_;
    Item defaultItem_; // Returned when item not found
};

} // namespace d2::game