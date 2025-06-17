#pragma once

#include "ui/ui_element.h"
#include "ui/ui_manager.h"
#include "game/item.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace d2 {

using d2::game::Item;

class InventoryScreen : public UIElement {
public:
    InventoryScreen();
    virtual ~InventoryScreen() = default;
    
    // Inventory dimensions
    int getInventoryWidth() const { return inventory_width_; }
    int getInventoryHeight() const { return inventory_height_; }
    
    // Item management
    bool addItem(std::shared_ptr<Item> item, const glm::ivec2& position);
    std::shared_ptr<Item> getItemAt(const glm::ivec2& position) const;
    bool removeItem(const glm::ivec2& position);
    
    // Selection and movement
    void selectSlot(int x, int y);
    glm::ivec2 getSelectedPosition() const { return selected_position_; }
    void moveSelection(int x, int y);
    void confirmMove();
    
    // Controller input
    bool handleControllerInput(ControllerAction action);
    
private:
    static const int inventory_width_ = 10;
    static const int inventory_height_ = 4;
    
    // Inventory grid - stores item references
    std::vector<std::vector<std::shared_ptr<Item>>> inventory_grid_;
    
    // Selection state
    glm::ivec2 selected_position_{-1, -1};
    glm::ivec2 move_target_{-1, -1};
    std::shared_ptr<Item> selected_item_;
    
    // Helper methods
    bool isValidPosition(const glm::ivec2& position) const;
    bool canPlaceItem(std::shared_ptr<Item> item, const glm::ivec2& position) const;
    void placeItemInGrid(std::shared_ptr<Item> item, const glm::ivec2& position);
    void clearItemFromGrid(const glm::ivec2& position);
};

} // namespace d2