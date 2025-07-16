#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <glm/vec2.hpp>

namespace d2::game {

enum class WaypointArea {
    ROGUE_ENCAMPMENT = 0,
    COLD_PLAINS = 1,
    STONY_FIELD = 2,
    DARK_WOOD = 3,
    BLACK_MARSH = 4,
    OUTER_CLOISTER = 5,
    JAIL_LEVEL_1 = 6,
    INNER_CLOISTER = 7,
    CATACOMBS_LEVEL_2 = 8,
    
    // Act 2
    LUT_GHOLEIN = 9,
    SEWERS_LEVEL_2 = 10,
    DRY_HILLS = 11,
    HALLS_OF_THE_DEAD_LEVEL_2 = 12,
    FAR_OASIS = 13,
    LOST_CITY = 14,
    PALACE_CELLAR_LEVEL_1 = 15,
    ARCANE_SANCTUARY = 16,
    CANYON_OF_THE_MAGI = 17,
    
    // Act 3
    KURAST_DOCKS = 18,
    SPIDER_FOREST = 19,
    GREAT_MARSH = 20,
    FLAYER_JUNGLE = 21,
    LOWER_KURAST = 22,
    KURAST_BAZAAR = 23,
    UPPER_KURAST = 24,
    TRAVINCAL = 25,
    DURANCE_OF_HATE_LEVEL_2 = 26,
    
    // Act 4
    THE_PANDEMONIUM_FORTRESS = 27,
    CITY_OF_THE_DAMNED = 28,
    RIVER_OF_FLAME = 29,
    
    // Act 5
    HARROGATH = 30,
    FRIGID_HIGHLANDS = 31,
    ARREAT_PLATEAU = 32,
    CRYSTALLINE_PASSAGE = 33,
    HALLS_OF_PAIN = 34,
    GLACIAL_TRAIL = 35,
    FROZEN_CREEPER = 36,
    THE_WORLDSTONE_KEEP_LEVEL_2 = 37,
    
    UNKNOWN = 255
};

class Waypoint {
public:
    Waypoint(WaypointArea area, const std::string& name, const glm::vec2& position);
    ~Waypoint() = default;

    WaypointArea getArea() const { return area_; }
    const std::string& getName() const { return name_; }
    glm::vec2 getPosition() const { return position_; }
    
    bool isActivated() const { return activated_; }
    void activate() { activated_ = true; }
    void deactivate() { activated_ = false; }
    
    int getAct() const;
    
    // Serialization support
    uint32_t getSerializationId() const;
    static WaypointArea fromSerializationId(uint32_t id);

private:
    WaypointArea area_;
    std::string name_;
    glm::vec2 position_;
    bool activated_;
};

class WaypointManager {
public:
    WaypointManager();
    ~WaypointManager() = default;
    
    // Waypoint management
    void addWaypoint(std::shared_ptr<Waypoint> waypoint);
    std::shared_ptr<Waypoint> getWaypoint(WaypointArea area) const;
    std::vector<std::shared_ptr<Waypoint>> getAllWaypoints() const;
    std::vector<std::shared_ptr<Waypoint>> getWaypointsByAct(int act) const;
    
    // Activation
    bool activateWaypoint(WaypointArea area);
    bool isWaypointActivated(WaypointArea area) const;
    
    // Fast travel
    bool canTravelTo(WaypointArea area) const;
    bool travelTo(WaypointArea area);
    
    // Utility
    size_t getActivatedCount() const;
    size_t getTotalCount() const;
    
    // Save/Load
    std::vector<uint32_t> getActivatedWaypointIds() const;
    void loadActivatedWaypoints(const std::vector<uint32_t>& ids);
    
    // Initialize default waypoints
    void initializeDefaultWaypoints();

private:
    std::vector<std::shared_ptr<Waypoint>> waypoints_;
    
    std::shared_ptr<Waypoint> findWaypoint(WaypointArea area) const;
};

} // namespace d2::game