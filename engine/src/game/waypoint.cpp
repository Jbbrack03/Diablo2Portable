#include "game/waypoint.h"
#include <algorithm>

namespace d2::game {

Waypoint::Waypoint(WaypointArea area, const std::string& name, const glm::vec2& position)
    : area_(area), name_(name), position_(position), activated_(false) {
}

int Waypoint::getAct() const {
    // Map waypoint areas to acts
    if (area_ >= WaypointArea::ROGUE_ENCAMPMENT && area_ <= WaypointArea::CATACOMBS_LEVEL_2) {
        return 1;
    } else if (area_ >= WaypointArea::LUT_GHOLEIN && area_ <= WaypointArea::CANYON_OF_THE_MAGI) {
        return 2;
    } else if (area_ >= WaypointArea::KURAST_DOCKS && area_ <= WaypointArea::DURANCE_OF_HATE_LEVEL_2) {
        return 3;
    } else if (area_ >= WaypointArea::THE_PANDEMONIUM_FORTRESS && area_ <= WaypointArea::RIVER_OF_FLAME) {
        return 4;
    } else if (area_ >= WaypointArea::HARROGATH && area_ <= WaypointArea::THE_WORLDSTONE_KEEP_LEVEL_2) {
        return 5;
    }
    return 0; // Unknown act
}

uint32_t Waypoint::getSerializationId() const {
    return static_cast<uint32_t>(area_);
}

WaypointArea Waypoint::fromSerializationId(uint32_t id) {
    if (id <= static_cast<uint32_t>(WaypointArea::THE_WORLDSTONE_KEEP_LEVEL_2)) {
        return static_cast<WaypointArea>(id);
    }
    return WaypointArea::UNKNOWN;
}

// WaypointManager Implementation
WaypointManager::WaypointManager() {
}

void WaypointManager::addWaypoint(std::shared_ptr<Waypoint> waypoint) {
    waypoints_.push_back(waypoint);
}

std::shared_ptr<Waypoint> WaypointManager::getWaypoint(WaypointArea area) const {
    return findWaypoint(area);
}

std::vector<std::shared_ptr<Waypoint>> WaypointManager::getAllWaypoints() const {
    return waypoints_;
}

std::vector<std::shared_ptr<Waypoint>> WaypointManager::getWaypointsByAct(int act) const {
    std::vector<std::shared_ptr<Waypoint>> result;
    
    for (const auto& waypoint : waypoints_) {
        if (waypoint->getAct() == act) {
            result.push_back(waypoint);
        }
    }
    
    return result;
}

bool WaypointManager::activateWaypoint(WaypointArea area) {
    auto waypoint = findWaypoint(area);
    if (waypoint) {
        waypoint->activate();
        return true;
    }
    return false;
}

bool WaypointManager::isWaypointActivated(WaypointArea area) const {
    auto waypoint = findWaypoint(area);
    return waypoint && waypoint->isActivated();
}

bool WaypointManager::canTravelTo(WaypointArea area) const {
    return isWaypointActivated(area);
}

bool WaypointManager::travelTo(WaypointArea area) {
    if (canTravelTo(area)) {
        // In a real implementation, this would trigger the travel logic
        // For now, just return true to indicate successful travel
        return true;
    }
    return false;
}

size_t WaypointManager::getActivatedCount() const {
    return std::count_if(waypoints_.begin(), waypoints_.end(),
                        [](const std::shared_ptr<Waypoint>& waypoint) {
                            return waypoint->isActivated();
                        });
}

size_t WaypointManager::getTotalCount() const {
    return waypoints_.size();
}

std::vector<uint32_t> WaypointManager::getActivatedWaypointIds() const {
    std::vector<uint32_t> result;
    
    for (const auto& waypoint : waypoints_) {
        if (waypoint->isActivated()) {
            result.push_back(waypoint->getSerializationId());
        }
    }
    
    return result;
}

void WaypointManager::loadActivatedWaypoints(const std::vector<uint32_t>& ids) {
    for (uint32_t id : ids) {
        WaypointArea area = Waypoint::fromSerializationId(id);
        if (area != WaypointArea::UNKNOWN) {
            activateWaypoint(area);
        }
    }
}

void WaypointManager::initializeDefaultWaypoints() {
    // Act 1 waypoints
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::ROGUE_ENCAMPMENT, "Rogue Encampment", glm::vec2(100, 100)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::COLD_PLAINS, "Cold Plains", glm::vec2(200, 150)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::STONY_FIELD, "Stony Field", glm::vec2(300, 200)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::DARK_WOOD, "Dark Wood", glm::vec2(400, 250)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::BLACK_MARSH, "Black Marsh", glm::vec2(500, 300)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::OUTER_CLOISTER, "Outer Cloister", glm::vec2(600, 350)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::JAIL_LEVEL_1, "Jail Level 1", glm::vec2(700, 400)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::INNER_CLOISTER, "Inner Cloister", glm::vec2(800, 450)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::CATACOMBS_LEVEL_2, "Catacombs Level 2", glm::vec2(900, 500)));
    
    // Act 2 waypoints
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::LUT_GHOLEIN, "Lut Gholein", glm::vec2(1000, 100)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::SEWERS_LEVEL_2, "Sewers Level 2", glm::vec2(1100, 150)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::DRY_HILLS, "Dry Hills", glm::vec2(1200, 200)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::HALLS_OF_THE_DEAD_LEVEL_2, "Halls of the Dead Level 2", glm::vec2(1300, 250)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::FAR_OASIS, "Far Oasis", glm::vec2(1400, 300)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::LOST_CITY, "Lost City", glm::vec2(1500, 350)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::PALACE_CELLAR_LEVEL_1, "Palace Cellar Level 1", glm::vec2(1600, 400)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::ARCANE_SANCTUARY, "Arcane Sanctuary", glm::vec2(1700, 450)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::CANYON_OF_THE_MAGI, "Canyon of the Magi", glm::vec2(1800, 500)));
    
    // Act 3 waypoints
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::KURAST_DOCKS, "Kurast Docks", glm::vec2(2000, 100)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::SPIDER_FOREST, "Spider Forest", glm::vec2(2100, 150)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::GREAT_MARSH, "Great Marsh", glm::vec2(2200, 200)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::FLAYER_JUNGLE, "Flayer Jungle", glm::vec2(2300, 250)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::LOWER_KURAST, "Lower Kurast", glm::vec2(2400, 300)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::KURAST_BAZAAR, "Kurast Bazaar", glm::vec2(2500, 350)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::UPPER_KURAST, "Upper Kurast", glm::vec2(2600, 400)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::TRAVINCAL, "Travincal", glm::vec2(2700, 450)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::DURANCE_OF_HATE_LEVEL_2, "Durance of Hate Level 2", glm::vec2(2800, 500)));
    
    // Act 4 waypoints
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::THE_PANDEMONIUM_FORTRESS, "The Pandemonium Fortress", glm::vec2(3000, 100)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::CITY_OF_THE_DAMNED, "City of the Damned", glm::vec2(3100, 150)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::RIVER_OF_FLAME, "River of Flame", glm::vec2(3200, 200)));
    
    // Act 5 waypoints
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::HARROGATH, "Harrogath", glm::vec2(4000, 100)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::FRIGID_HIGHLANDS, "Frigid Highlands", glm::vec2(4100, 150)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::ARREAT_PLATEAU, "Arreat Plateau", glm::vec2(4200, 200)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::CRYSTALLINE_PASSAGE, "Crystalline Passage", glm::vec2(4300, 250)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::HALLS_OF_PAIN, "Halls of Pain", glm::vec2(4400, 300)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::GLACIAL_TRAIL, "Glacial Trail", glm::vec2(4500, 350)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::FROZEN_CREEPER, "Frozen Creeper", glm::vec2(4600, 400)));
    addWaypoint(std::make_shared<Waypoint>(WaypointArea::THE_WORLDSTONE_KEEP_LEVEL_2, "The Worldstone Keep Level 2", glm::vec2(4700, 450)));
}

std::shared_ptr<Waypoint> WaypointManager::findWaypoint(WaypointArea area) const {
    auto it = std::find_if(waypoints_.begin(), waypoints_.end(),
                          [area](const std::shared_ptr<Waypoint>& waypoint) {
                              return waypoint->getArea() == area;
                          });
    return (it != waypoints_.end()) ? *it : nullptr;
}

} // namespace d2::game