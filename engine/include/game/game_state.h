#pragma once

namespace d2::game {

class GameState {
public:
    GameState() = default;
    
    bool hasPlayer() const;
};

} // namespace d2::game