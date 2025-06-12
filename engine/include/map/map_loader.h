#pragma once

#include <string>
#include <memory>

namespace d2::map {

class Map {
public:
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isWalkable(int x, int y) const;

    // Friend class for testing access
    friend class MapLoader;

private:
    int m_width = 0;
    int m_height = 0;
    // Additional map data will be added as needed
};

class MapLoader {
public:
    std::unique_ptr<Map> loadMap(const std::string& filename);
};

} // namespace d2::map