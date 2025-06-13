#pragma once

#include <string>
#include <memory>
#include <vector>

namespace d2::map {

class Map {
public:
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isWalkable(int x, int y) const;

    // Friend classes for testing access
    friend class MapLoader;
    friend class TestMap;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<std::vector<bool>> m_walkableGrid;  // 2D grid of walkability
};

class MapLoader {
public:
    std::unique_ptr<Map> loadMap(const std::string& filename);
};

} // namespace d2::map