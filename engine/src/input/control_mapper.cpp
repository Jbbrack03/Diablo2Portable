#include "input/control_mapper.h"
#include <cmath>

namespace d2::input {

void ControlMapper::setScheme(ControlScheme scheme) {
    scheme_ = scheme;
}

Action ControlMapper::processInput(const AnalogInput& input, ButtonState buttons) {
    Action action;
    
    if (scheme_ == ControlScheme::DIRECT) {
        // Check if stick is moved enough
        float magnitude = std::sqrt(input.x * input.x + input.y * input.y);
        if (magnitude > 0.1f) {
            action.type = ActionType::MOVE;
            action.direction = quantizeToD2Direction(input.x, input.y);
        }
    }
    
    return action;
}

Direction ControlMapper::quantizeToD2Direction(float x, float y) const {
    // Convert analog stick to 8-way direction
    float angle = std::atan2(y, x);
    
    // Convert to degrees and normalize to 0-360
    float degrees = angle * 180.0f / M_PI;
    if (degrees < 0) degrees += 360.0f;
    
    // Quantize to 8 directions (45 degree sectors)
    int sector = static_cast<int>((degrees + 22.5f) / 45.0f) % 8;
    
    // Map sectors to directions (0 = East, going counter-clockwise)
    switch (sector) {
        case 0: return Direction::EAST;
        case 1: return Direction::NORTHEAST;
        case 2: return Direction::NORTH;
        case 3: return Direction::NORTHWEST;
        case 4: return Direction::WEST;
        case 5: return Direction::SOUTHWEST;
        case 6: return Direction::SOUTH;
        case 7: return Direction::SOUTHEAST;
        default: return Direction::NORTH;
    }
}

} // namespace d2::input