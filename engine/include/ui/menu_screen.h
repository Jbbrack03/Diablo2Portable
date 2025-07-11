#pragma once

#include <vector>
#include <functional>
#include <cassert>

// Forward declaration - the full definition will be included where needed
namespace d2 {
enum class ControllerAction;
}

namespace d2::ui {

/**
 * @brief Template base class for menu-based UI screens
 * 
 * This class provides common functionality for screens that display
 * a list of selectable items with keyboard/controller navigation.
 * It eliminates code duplication across menu screens while allowing
 * customization through template parameters and virtual methods.
 * 
 * @tparam ActionType Enum type representing menu actions
 * @tparam ItemType Type of items stored in the menu
 */
template<typename ActionType, typename ItemType>
class MenuScreen {
public:
    MenuScreen() = default;
    virtual ~MenuScreen() = default;
    
    // Non-copyable but movable
    MenuScreen(const MenuScreen&) = delete;
    MenuScreen& operator=(const MenuScreen&) = delete;
    MenuScreen(MenuScreen&&) = default;
    MenuScreen& operator=(MenuScreen&&) = default;
    
    /**
     * @brief Navigate to the next enabled item
     * Wraps around to the first item if at the end
     */
    virtual void navigateToNext() {
        if (items_.empty()) return;
        
        size_t next_index = findNextEnabledItem(selected_index_, true);
        if (next_index != selected_index_) {
            selected_index_ = next_index;
            onSelectionChanged(selected_index_);
        }
    }
    
    /**
     * @brief Navigate to the previous enabled item
     * Wraps around to the last item if at the beginning
     */
    virtual void navigateToPrevious() {
        if (items_.empty()) return;
        
        size_t prev_index = findNextEnabledItem(selected_index_, false);
        if (prev_index != selected_index_) {
            selected_index_ = prev_index;
            onSelectionChanged(selected_index_);
        }
    }
    
    /**
     * @brief Activate the currently selected item
     * Calls onItemActivated if the item is enabled
     */
    virtual void activateSelected() {
        if (selected_index_ < items_.size() && isItemEnabled(selected_index_)) {
            onItemActivated(selected_index_);
        }
    }
    
    /**
     * @brief Select a specific item by index
     * @param index Index of the item to select
     */
    void selectItem(size_t index) {
        if (index < items_.size()) {
            selected_index_ = index;
            onSelectionChanged(selected_index_);
        }
    }
    
    /**
     * @brief Get the currently selected item index
     * @return Index of the selected item
     */
    size_t getSelectedIndex() const {
        return selected_index_;
    }
    
    /**
     * @brief Get the current selected action
     * @return Current selected action
     */
    ActionType getSelectedAction() const {
        return selected_action_;
    }
    
    /**
     * @brief Handle controller input for menu navigation
     * @param action Controller action to handle
     * @return true if action was handled, false otherwise
     */
    virtual bool handleControllerInput(d2::ControllerAction action);
    
    /**
     * @brief Add an item to the menu
     * @param item Item to add
     */
    void addItem(const ItemType& item) {
        items_.push_back(item);
    }
    
    /**
     * @brief Add an item to the menu (move version)
     * @param item Item to add
     */
    void addItem(ItemType&& item) {
        items_.push_back(std::move(item));
    }
    
    /**
     * @brief Get the number of items in the menu
     * @return Number of items
     */
    size_t getItemCount() const {
        return items_.size();
    }
    
    /**
     * @brief Get an item by index
     * @param index Index of the item
     * @return Reference to the item
     */
    const ItemType& getItem(size_t index) const {
        assert(index < items_.size());
        return items_[index];
    }
    
    /**
     * @brief Get mutable reference to an item by index
     * @param index Index of the item
     * @return Mutable reference to the item
     */
    ItemType& getItem(size_t index) {
        assert(index < items_.size());
        return items_[index];
    }
    
    /**
     * @brief Clear all items from the menu
     */
    void clearItems() {
        items_.clear();
        selected_index_ = 0;
        selected_action_ = ActionType{};
    }
    
    /**
     * @brief Check if the menu is empty
     * @return true if no items, false otherwise
     */
    bool isEmpty() const {
        return items_.empty();
    }

protected:
    /**
     * @brief Check if an item is enabled
     * Override this method to implement custom enable/disable logic
     * @param index Index of the item to check
     * @return true if item is enabled, false otherwise
     */
    virtual bool isItemEnabled(size_t index) const {
        return index < items_.size();
    }
    
    /**
     * @brief Called when an item is activated
     * Override this method to handle item activation
     * @param index Index of the activated item
     */
    virtual void onItemActivated(size_t index) {
        // Default implementation does nothing
        // Derived classes should override this
    }
    
    /**
     * @brief Called when the selection changes
     * Override this method to handle selection changes
     * @param index New selected index
     */
    virtual void onSelectionChanged(size_t index) {
        // Default implementation does nothing
        // Derived classes can override this for custom behavior
    }
    
    /**
     * @brief Find the next enabled item in the given direction
     * @param start Starting index
     * @param forward true for forward direction, false for backward
     * @return Index of the next enabled item
     */
    virtual size_t findNextEnabledItem(size_t start, bool forward) const {
        if (items_.empty()) return 0;
        
        size_t current = start;
        size_t count = 0;
        
        do {
            if (forward) {
                current = (current + 1) % items_.size();
            } else {
                current = (current == 0) ? items_.size() - 1 : current - 1;
            }
            count++;
            
            if (isItemEnabled(current)) {
                return current;
            }
        } while (count < items_.size());
        
        // If no enabled items found, return the original index
        return start;
    }
    
    // Data members
    std::vector<ItemType> items_;
    size_t selected_index_{0};
    ActionType selected_action_{};
};

} // namespace d2::ui