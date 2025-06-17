#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace d2::map {

struct MapObject {
    std::string type;
    int x, y;
    std::unordered_map<std::string, std::string> properties;
};

class Map {
public:
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isWalkable(int x, int y) const;

    // Random generation features
    bool hasEntrance() const { return m_hasEntrance; }
    bool hasExit() const { return m_hasExit; }
    glm::ivec2 getEntrance() const { return m_entrance; }
    glm::ivec2 getExit() const { return m_exit; }

    // Multi-layer support
    int getLayerCount() const { return m_layers.size(); }
    bool hasLayer(const std::string& layerName) const;
    int getLayerWidth(const std::string& layerName) const;
    int getLayerHeight(const std::string& layerName) const;

    // Interactive objects
    std::vector<MapObject> getInteractiveObjects() const { return m_objects; }

    // Friend classes for testing access
    friend class MapLoader;
    friend class TestMap;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<std::vector<bool>> m_walkableGrid;  // 2D grid of walkability
    
    // Random generation data
    bool m_hasEntrance = false;
    bool m_hasExit = false;
    glm::ivec2 m_entrance{0, 0};
    glm::ivec2 m_exit{0, 0};
    
    // Layer system
    std::unordered_map<std::string, std::vector<std::vector<int>>> m_layers;
    
    // Interactive objects
    std::vector<MapObject> m_objects;
};

class MapLoader {
public:
    std::unique_ptr<Map> loadMap(const std::string& filename);
    std::unique_ptr<Map> generateRandomMap(int width, int height, uint32_t seed);
};

} // namespace d2::map