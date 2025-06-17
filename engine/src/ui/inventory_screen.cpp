#include "ui/inventory_screen.h"

namespace d2 {

InventoryScreen::InventoryScreen() {
    // Initialize inventory grid
    inventory_grid_.resize(inventory_height_);
    for (auto& row : inventory_grid_) {
        row.resize(inventory_width_);
    }
}

bool InventoryScreen::addItem(std::shared_ptr<Item> item, const glm::ivec2& position) {
    if (!item || !isValidPosition(position)) {
        return false;
    }
    
    if (!canPlaceItem(item, position)) {
        return false;
    }
    
    placeItemInGrid(item, position);
    return true;
}

std::shared_ptr<Item> InventoryScreen::getItemAt(const glm::ivec2& position) const {
    if (!isValidPosition(position)) {
        return nullptr;
    }
    
    return inventory_grid_[position.y][position.x];
}

bool InventoryScreen::removeItem(const glm::ivec2& position) {
    if (!isValidPosition(position)) {
        return false;
    }
    
    auto item = getItemAt(position);
    if (!item) {
        return false;
    }
    
    clearItemFromGrid(position);
    return true;
}

void InventoryScreen::selectSlot(int x, int y) {
    selected_position_ = glm::ivec2(x, y);
    selected_item_ = getItemAt(selected_position_);
}

void InventoryScreen::moveSelection(int x, int y) {
    move_target_ = glm::ivec2(x, y);
}

void InventoryScreen::confirmMove() {
    if (selected_item_ && isValidPosition(selected_position_) && isValidPosition(move_target_)) {
        if (canPlaceItem(selected_item_, move_target_)) {
            // Remove from old position
            clearItemFromGrid(selected_position_);
            
            // Place at new position
            placeItemInGrid(selected_item_, move_target_);
            
            // Update selection
            selected_position_ = move_target_;
        }
    }
    
    // Clear move targets
    move_target_ = glm::ivec2(-1, -1);
}

bool InventoryScreen::handleControllerInput(ControllerAction action) {
    switch (action) {
        case ControllerAction::NAVIGATE_UP:
            if (selected_position_.y > 0) {
                selected_position_.y--;
            } else if (selected_position_ == glm::ivec2(-1, -1)) {
                selected_position_ = glm::ivec2(0, 0);
            }
            break;
            
        case ControllerAction::NAVIGATE_DOWN:
            if (selected_position_ == glm::ivec2(-1, -1)) {
                selected_position_ = glm::ivec2(0, 0);
            } else if (selected_position_.y < inventory_height_ - 1) {
                selected_position_.y++;
            }
            break;
            
        case ControllerAction::NAVIGATE_LEFT:
            if (selected_position_.x > 0) {
                selected_position_.x--;
            }
            break;
            
        case ControllerAction::NAVIGATE_RIGHT:
            if (selected_position_.x < inventory_width_ - 1) {
                selected_position_.x++;
            }
            break;
            
        case ControllerAction::ACTION_BUTTON:
        case ControllerAction::CANCEL_BUTTON:
            // Handle action/cancel buttons
            break;
    }
    
    return true;
}

bool InventoryScreen::isValidPosition(const glm::ivec2& position) const {
    return position.x >= 0 && position.x < inventory_width_ &&
           position.y >= 0 && position.y < inventory_height_;
}

bool InventoryScreen::canPlaceItem(std::shared_ptr<Item> item, const glm::ivec2& position) const {
    if (!item || !isValidPosition(position)) {
        return false;
    }
    
    // Check if item fits within grid bounds
    glm::ivec2 itemSize(item->getWidth(), item->getHeight());
    if (position.x + itemSize.x > inventory_width_ ||
        position.y + itemSize.y > inventory_height_) {
        return false;
    }
    
    // Check if all required slots are empty
    for (int y = position.y; y < position.y + itemSize.y; ++y) {
        for (int x = position.x; x < position.x + itemSize.x; ++x) {
            if (inventory_grid_[y][x] != nullptr) {
                return false;
            }
        }
    }
    
    return true;
}

void InventoryScreen::placeItemInGrid(std::shared_ptr<Item> item, const glm::ivec2& position) {
    if (!item || !isValidPosition(position)) {
        return;
    }
    
    glm::ivec2 itemSize(item->getWidth(), item->getHeight());
    
    // Place item reference in all occupied slots
    for (int y = position.y; y < position.y + itemSize.y; ++y) {
        for (int x = position.x; x < position.x + itemSize.x; ++x) {
            inventory_grid_[y][x] = item;
        }
    }
}

void InventoryScreen::clearItemFromGrid(const glm::ivec2& position) {
    auto item = getItemAt(position);
    if (!item) {
        return;
    }
    
    glm::ivec2 itemSize(item->getWidth(), item->getHeight());
    
    // Find the top-left corner of the item
    glm::ivec2 topLeft = position;
    for (int y = 0; y <= position.y; ++y) {
        for (int x = 0; x <= position.x; ++x) {
            if (isValidPosition(glm::ivec2(x, y)) && 
                inventory_grid_[y][x] == item) {
                topLeft = glm::ivec2(x, y);
                break;
            }
        }
    }
    
    // Clear all slots occupied by this item
    for (int y = topLeft.y; y < topLeft.y + itemSize.y && y < inventory_height_; ++y) {
        for (int x = topLeft.x; x < topLeft.x + itemSize.x && x < inventory_width_; ++x) {
            if (inventory_grid_[y][x] == item) {
                inventory_grid_[y][x] = nullptr;
            }
        }
    }
}

} // namespace d2