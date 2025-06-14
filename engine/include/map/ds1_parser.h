#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace d2::map {

// Represents a single tile in a DS1 layer
struct DS1Tile {
    uint32_t prop1 = 0;
    uint32_t prop2 = 0;
    uint32_t prop3 = 0;
    uint32_t prop4 = 0;
    uint8_t orientation = 0;
    uint32_t mainIndex = 0;
    uint32_t subIndex = 0;
    uint32_t unknown = 0;
};

// Represents a layer in the DS1 file
class DS1Layer {
public:
    DS1Layer(int width, int height) : m_width(width), m_height(height) {
        m_tiles.resize(width * height);
    }
    
    DS1Tile getTile(int x, int y) const {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            return m_tiles[y * m_width + x];
        }
        return DS1Tile{};
    }
    
    void setTile(int x, int y, const DS1Tile& tile) {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            m_tiles[y * m_width + x] = tile;
        }
    }
    
private:
    int m_width;
    int m_height;
    std::vector<DS1Tile> m_tiles;
};

// Represents a parsed DS1 file
class DS1File {
public:
    int getVersion() const { return m_version; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getAct() const { return m_act; }
    
    int getWallLayerCount() const { return m_wallLayerCount; }
    int getFloorLayerCount() const { return m_floorLayerCount; }
    bool hasShadowLayer() const { return m_hasShadowLayer; }
    
    std::shared_ptr<DS1Layer> getFloorLayer(int index) const {
        if (index >= 0 && index < static_cast<int>(m_floorLayers.size())) {
            return m_floorLayers[index];
        }
        return nullptr;
    }
    
    std::shared_ptr<DS1Layer> getWallLayer(int index) const {
        if (index >= 0 && index < static_cast<int>(m_wallLayers.size())) {
            return m_wallLayers[index];
        }
        return nullptr;
    }
    
    // Friend class for parser access
    friend class DS1Parser;
    
private:
    int m_version = 0;
    int m_width = 0;
    int m_height = 0;
    int m_act = 0;
    int m_wallLayerCount = 0;
    int m_floorLayerCount = 0;
    bool m_hasShadowLayer = false;
    
    std::vector<std::shared_ptr<DS1Layer>> m_floorLayers;
    std::vector<std::shared_ptr<DS1Layer>> m_wallLayers;
    std::shared_ptr<DS1Layer> m_shadowLayer;
};

// Parser for DS1 files
class DS1Parser {
public:
    std::unique_ptr<DS1File> parse(const std::vector<uint8_t>& data);
    std::unique_ptr<DS1File> loadFromFile(const std::string& filename);
};

} // namespace d2::map