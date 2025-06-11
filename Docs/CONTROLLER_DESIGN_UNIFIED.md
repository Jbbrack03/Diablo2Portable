# Diablo II Android - Unified Controller Design Document

## Overview
This document consolidates all controller and UI design specifications for the Diablo II Android port, optimized for the Retroid Pocket Flip 2 and similar gamepad-equipped devices.

## Controller Philosophy

### Core Design Principles
1. **Direct Control**: Character responds immediately to stick input
2. **Smart Assistance**: Intelligent targeting and context-aware actions
3. **Minimal Friction**: Reduce clicks/buttons needed for common actions
4. **Preserve Depth**: Maintain tactical complexity of original game

### The Fundamental Shift
- **Original**: Mouse position determines action location
- **Our Design**: Direct character control with intelligent target selection

## Control Scheme

### Primary Controls

#### Movement System
- **Left Analog Stick**: Direct 8-directional character movement
  - Analog sensitivity for walk/run without toggle
  - Quick tap = single step
  - Hold = continuous movement
  - Smart pathfinding prevents getting stuck on obstacles

#### Targeting System
- **Right Analog Stick**: Manual cursor override/precision targeting
  - Free cursor control for spells/ranged attacks
  - Auto-centers when released
  - Cursor speed adjustable in settings
  - Soft magnetism to valid targets

### Combat Controls

#### Face Buttons
- **A Button (Primary Action)**
  - Attack with equipped weapon/skill
  - Interact with NPCs/objects
  - Pick up items (context-sensitive)
  - Confirm in menus

- **B Button (Secondary)**
  - Secondary skill (right-click equivalent)
  - Cancel/back in menus
  - Quick escape from dialogues

- **X Button (Utility)**
  - Force move without attacking
  - Hold: Show item labels on ground
  - Double tap: Loot all nearby items

- **Y Button (Toggle)**
  - Cycle through run/walk modes
  - Hold: Character stats overlay

### Skill System

#### Quick Access Skills (8 slots)
- **L2 + Face Buttons**: Skills 1-4
- **R2 + Face Buttons**: Skills 5-8
- No mode switching required - instant cast

#### Resource Management
- **L1**: Use health potion from belt
- **R1**: Use mana potion from belt
- **L1 + R1**: Use rejuvenation potion

### Menu Navigation

#### D-Pad Functions
- **Up**: Character screen
- **Down**: Skill tree
- **Left**: Inventory
- **Right**: Party screen (multiplayer)

#### System Buttons
- **Start**: Game menu (Save/Exit/Options)
- **Select**: Toggle automap
- **Select (Hold)**: Quest log

### Advanced Controls

#### Analog Stick Clicks
- **L3**: Toggle item comparison mode
- **R3**: Center camera on character

#### Combat Modifiers
- **Hold L2**: Strafe mode (lock facing direction)
- **Hold R2**: Force stand still (for ranged attacks)

#### Special Combinations
- **L2 + R2 + A**: Cast Town Portal
- **L2 + R2 + B**: Identify item (when Cain unavailable)

## Intelligent Systems

### Smart Targeting Algorithm

```cpp
class SmartTargeting {
    Entity* selectTarget(const GameState& state) {
        // 1. Maintain current target if valid
        if (currentTarget && isValid(currentTarget)) {
            return currentTarget;
        }
        
        // 2. Priority system
        auto candidates = getTargetsInRange();
        sortByPriority(candidates, {
            1.0f,  // Distance weight
            0.8f,  // Threat level weight
            0.6f,  // Damage dealt weight
            0.4f,  // Player facing weight
        });
        
        return candidates.empty() ? nullptr : candidates[0];
    }
};
```

### Context-Sensitive Actions

The A button adapts based on context:

1. **Near Enemy**: Attack
2. **Near Item**: Pick up
3. **Near NPC**: Talk
4. **Near Door/Chest**: Open
5. **Near Waypoint**: Activate
6. **Empty Space**: Move (if no target)

### Item Pickup System

#### Single Item
- Walk near item
- Press A to pick up

#### Item Pile
- Walk near pile
- Press X to open radial selector
- Left stick to choose
- A to confirm

#### Smart Filters
- Auto-pickup gold
- Auto-pickup potions when low
- Configurable quality filters

## UI Adaptations

### Screen Layout (1080p)

```
┌─────────────────────────────────────────────────────┐
│ HP                  Game View              Mana      │
│ ┌──┐                                       ┌──┐      │
│ │  │  ┌─────────────────────────────────┐ │  │      │
│ │  │  │                                 │ │  │      │
│ └──┘  │         Gameplay Area          │ └──┘      │
│       │                                 │           │
│Skills │                                 │ Belt      │
│┌────┐ └─────────────────────────────────┘ ┌────┐   │
││1234│                                     │PPPP│    │
│└────┘  [L2+ABXY]     XP Bar      [R2+ABXY] └────┘   │
└─────────────────────────────────────────────────────┘
```

### HUD Elements

#### Health/Mana Orbs
- Moved inward for widescreen
- Larger hit detection for touch
- Visual pulse on potion use

#### Skill Bar
- 8 slots always visible
- Shows button combinations
- Cooldown timers
- Mana cost indicators

#### Belt
- Simplified 4-slot design
- Auto-refill from inventory
- Type indicators (health/mana/rejuv)

### Inventory Redesign

#### Grid Navigation
- D-pad moves between slots
- Hold to move faster
- A to pick up/put down
- X to drop
- Y for details

#### Visual Enhancements
- Highlight current slot
- Show valid equipment slots
- Color coding by rarity
- Stack numbers for consumables

### Menu Systems

All menus redesigned for controller:
- Clear focus indicators
- Consistent button mapping
- No mouse-only interactions
- Smooth scrolling lists

## Implementation Details

### Dead Zones and Sensitivity

```cpp
struct ControllerConfig {
    float leftStickDeadZone = 0.15f;
    float rightStickDeadZone = 0.20f;
    float cursorSpeed = 800.0f;
    float movementThreshold = 0.3f;
    
    // Accessibility options
    bool toggleRun = false;
    bool holdToMove = false;
    float autoTargetStrength = 0.7f;
};
```

### Input Buffering

- 3-frame input buffer for combos
- Skill queuing during animations
- Movement command buffering

### Accessibility Features

1. **Customizable Controls**
   - Full button remapping
   - Multiple preset schemes
   - Stick swapping

2. **Assist Options**
   - Auto-targeting strength slider
   - Sticky targeting toggle
   - One-button potion use

3. **Visual Aids**
   - Button prompt size options
   - Colorblind modes
   - High contrast UI option

## Control Schemes

### Scheme A: "Modern" (Default)
- Direct movement
- Smart targeting
- Context actions
- Recommended for new players

### Scheme B: "Classic"
- Left stick moves cursor
- A button clicks
- More like PC experience
- For veterans

### Scheme C: "Hybrid"
- Direct movement
- Manual targeting only
- No auto-aim
- For precision players

## Performance Considerations

### Input Latency
- Target: <16ms (1 frame)
- Predictive movement
- Client-side responsiveness

### Optimization
- Input thread separate from render
- Efficient collision detection
- Batched UI updates

## Testing Requirements

### Controller Compatibility
- Xbox-style controllers
- PlayStation controllers  
- Generic Android gamepads
- Touch fallback controls

### User Testing Metrics
- Time to complete actions
- Misclick rates
- Player satisfaction scores
- Accessibility compliance

## Migration from Mouse

### Learning Curve Mitigation
- Interactive tutorial
- Tooltips for controls
- Practice area
- Control scheme comparison

### Muscle Memory Translation
| Mouse Action | Controller Action |
|-------------|------------------|
| Left click | A button |
| Right click | B button |
| Shift+click | L2 + A |
| Show items | Hold X |
| Force move | X button |

## Future Enhancements

### Planned Features
1. Gesture support on touchpad
2. Haptic feedback integration
3. Voice commands for accessibility
4. Custom macro support

### Community Feedback Integration
- Control telemetry
- A/B testing different schemes
- Regular updates based on feedback

## Conclusion

This unified controller design maintains Diablo II's tactical depth while providing modern, intuitive controls. The system prioritizes player intention over precision, using intelligent assistance to bridge the gap between mouse and gamepad control.

The design is flexible enough to accommodate different play styles while remaining accessible to newcomers. Most importantly, it makes Diablo II feel like it was designed for controllers from the start.