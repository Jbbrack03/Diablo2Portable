# Diablo II Android - Controller and UI Design Document

## Controller Layout Overview

### Retroid Pocket Flip 2 Controller Mapping

The Retroid Pocket Flip 2 features a standard Xbox-style controller layout:
- Dual analog sticks
- D-pad
- Face buttons (A, B, X, Y)
- Shoulder buttons (L1, R1)
- Triggers (L2, R2)
- Start and Select buttons
- Clickable sticks (L3, R3)

## Control Scheme Design

### Movement and Targeting

#### Primary Movement
- **Left Analog Stick**: Direct character movement (8-directional)
  - Analog input provides variable movement speed
  - Holding for 0.5s in a direction = continuous movement
  - Quick tap = single step

#### Targeting System
- **Right Analog Stick**: Free cursor control
  - Used for precise targeting of spells and ranged attacks
  - Cursor speed adjustable in settings
  - Auto-centers when released

### Combat Controls

#### Basic Actions
- **A Button**: 
  - Primary action (left-click equivalent)
  - Attack with equipped weapon
  - Interact with NPCs/objects
  - Pick up items (when no enemies nearby)

- **B Button**:
  - Secondary skill (right-click equivalent)
  - Cancel action in menus
  - Quick escape from conversations

- **X Button**:
  - Force move (no attack)
  - Hold to show item labels on ground
  - Quick loot all nearby items (double tap)

- **Y Button**:
  - Toggle between run/walk
  - Hold for character stats overlay

### Skill System

#### Quick Skills (Combat Ready)
- **L2 + A**: Skill Slot 1
- **L2 + B**: Skill Slot 2
- **L2 + X**: Skill Slot 3
- **L2 + Y**: Skill Slot 4
- **R2 + A**: Skill Slot 5
- **R2 + B**: Skill Slot 6
- **R2 + X**: Skill Slot 7
- **R2 + Y**: Skill Slot 8

#### Potion Usage
- **L1**: Use health potion from belt
- **R1**: Use mana potion from belt
- **L1 + R1**: Use rejuvenation potion

### Menu Navigation

#### Quick Access
- **Start**: Game Menu (Save/Exit/Options)
- **Select**: Toggle Automap
- **Select (Hold)**: Quest Log

#### D-Pad Functions
- **D-Pad Up**: Character Screen
- **D-Pad Down**: Skill Tree
- **D-Pad Left**: Inventory
- **D-Pad Right**: Party Screen (Multiplayer)

### Advanced Controls

#### Stick Clicks
- **L3**: Toggle item comparison mode
- **R3**: Center camera on character

#### Combination Controls
- **L2 + R2 + A**: Town Portal
- **L2 + R2 + B**: Identify Item
- **Hold B + Move**: Force stand still (for ranged attacks)

## UI Modifications

### Screen Layout (1080p Widescreen)

```
┌─────────────────────────────────────────────────────────┐
│ HP Orb    Main Game View (16:9)           Mana Orb      │
│  ┌───┐                                        ┌───┐      │
│  │   │    ┌───────────────────────────┐      │   │      │
│  │   │    │                           │      │   │      │
│  │   │    │                           │      │   │      │
│  └───┘    │      Game World          │      └───┘      │
│           │                           │                  │
│ Skills    │                           │    Quick Inv    │
│ ┌─┬─┬─┬─┐ │                           │    ┌─┬─┬─┬─┐   │
│ │1│2│3│4│ └───────────────────────────┘    │ │ │ │ │   │
│ └─┴─┴─┴─┘                                  └─┴─┴─┴─┘   │
│           Belt: [P][P][P][P]                           │
│           XP Bar ═══════════════════                   │
└─────────────────────────────────────────────────────────┘
```

### HUD Elements

#### Health and Mana Orbs
- Moved slightly inward for widescreen
- Larger touch targets for mobile
- Visual feedback on potion use

#### Skill Bar
- 8 skill slots visible at all times
- Shows cooldowns and mana costs
- Controller button hints displayed

#### Belt Quick Bar
- Simplified 4-slot design
- Auto-refill from inventory
- Potion type indicators

#### Experience Bar
- Full width at bottom
- Shows progress to next level
- Combat text for XP gains

### Inventory Redesign

#### Grid Navigation
- Highlight system for controller navigation
- Smart cursor that jumps between items
- Category tabs (Equipment, Potions, Misc)

#### Item Management
- **A**: Pick up/equip item
- **X**: Drop item
- **Y**: Show item details
- **B**: Quick compare with equipped

#### Visual Improvements
- Larger item slots for visibility
- Color coding for item quality
- Stack counters for consumables

### Skill Tree Interface

#### Navigation
- Node-based navigation with D-pad
- Connecting lines show prerequisites
- Preview of skill effects

#### Point Allocation
- **A**: Allocate skill point
- **Y**: Preview at max level
- **X**: Assign to quick slot

### Character Screen

#### Stats Display
- Tabbed interface (Stats, Resistances, Skills)
- Comparative view with item changes
- Touch-friendly stat allocation

### Vendor Interface

#### Shop Navigation
- List view with categories
- Sort options (Price, Type, Level)
- Quick buy/sell with one button

#### Gambling Screen
- Larger item preview
- Success rate indicator
- Bulk gambling option

## Contextual UI System

### Combat Mode
- Minimal UI for better visibility
- Enemy health bars above targets
- Damage numbers with type indicators

### Town Mode
- Full UI with all panels accessible
- NPC dialogue in easy-read font
- Quest indicators on minimap

### Inventory Mode
- Darkened game world background
- Full-screen inventory management
- Comparison tooltips

## Visual Design Guidelines

### Color Scheme
- Maintain dark fantasy aesthetic
- High contrast for OLED screens
- Colorblind-friendly options

### Font Requirements
- Minimum 16pt for readability
- Exocol font for authenticity
- High DPI support

### Button Prompts
- Xbox-style button icons
- Context-sensitive hints
- Customizable prompt style

## Accessibility Features

### Control Options
- Button remapping
- Stick sensitivity adjustment
- Dead zone configuration
- Toggle/Hold options for all buttons

### Visual Aids
- Item label size options
- Colorblind modes
- UI scale adjustment
- Enhanced cursor visibility

### Gameplay Assists
- Auto-pickup radius setting
- Smart targeting assistance
- Skill queue system
- One-button potion use

## Performance Considerations

### UI Rendering
- Batch draw calls for UI elements
- Texture atlasing for icons
- Efficient font rendering
- 60 FPS target maintained

### Memory Usage
- Lazy loading of UI assets
- Compressed textures
- Efficient sprite caching

## Implementation Priority

1. **Phase 1**: Core control mapping
2. **Phase 2**: Basic UI adaptation
3. **Phase 3**: Inventory and skills interfaces
4. **Phase 4**: Polish and accessibility
5. **Phase 5**: Performance optimization

This design ensures Diablo II plays naturally with a controller while maintaining the depth and complexity of the original PC experience.