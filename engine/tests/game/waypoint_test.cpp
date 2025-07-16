#include <gtest/gtest.h>
#include "game/waypoint.h"
#include <memory>

namespace d2::game {

class WaypointTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<WaypointManager>();
        manager->initializeDefaultWaypoints();
    }
    
    void TearDown() override {
        // Test cleanup
    }
    
    std::unique_ptr<WaypointManager> manager;
};

// Phase 44: Missing Game Systems - Waypoint Implementation
TEST_F(WaypointTest, CreateWaypoint) {
    auto waypoint = std::make_shared<Waypoint>(WaypointArea::ROGUE_ENCAMPMENT, "Rogue Encampment", glm::vec2(100.0f, 200.0f));
    
    EXPECT_EQ(waypoint->getArea(), WaypointArea::ROGUE_ENCAMPMENT);
    EXPECT_EQ(waypoint->getName(), "Rogue Encampment");
    EXPECT_EQ(waypoint->getPosition(), glm::vec2(100.0f, 200.0f));
    EXPECT_FALSE(waypoint->isActivated());
    EXPECT_EQ(waypoint->getAct(), 1);
}

TEST_F(WaypointTest, WaypointActivation) {
    auto waypoint = std::make_shared<Waypoint>(WaypointArea::COLD_PLAINS, "Cold Plains", glm::vec2(50.0f, 100.0f));
    
    EXPECT_FALSE(waypoint->isActivated());
    
    waypoint->activate();
    EXPECT_TRUE(waypoint->isActivated());
    
    waypoint->deactivate();
    EXPECT_FALSE(waypoint->isActivated());
}

TEST_F(WaypointTest, WaypointActMapping) {
    auto act1_waypoint = std::make_shared<Waypoint>(WaypointArea::ROGUE_ENCAMPMENT, "Rogue Encampment", glm::vec2(0, 0));
    auto act2_waypoint = std::make_shared<Waypoint>(WaypointArea::LUT_GHOLEIN, "Lut Gholein", glm::vec2(0, 0));
    auto act3_waypoint = std::make_shared<Waypoint>(WaypointArea::KURAST_DOCKS, "Kurast Docks", glm::vec2(0, 0));
    auto act4_waypoint = std::make_shared<Waypoint>(WaypointArea::THE_PANDEMONIUM_FORTRESS, "The Pandemonium Fortress", glm::vec2(0, 0));
    auto act5_waypoint = std::make_shared<Waypoint>(WaypointArea::HARROGATH, "Harrogath", glm::vec2(0, 0));
    
    EXPECT_EQ(act1_waypoint->getAct(), 1);
    EXPECT_EQ(act2_waypoint->getAct(), 2);
    EXPECT_EQ(act3_waypoint->getAct(), 3);
    EXPECT_EQ(act4_waypoint->getAct(), 4);
    EXPECT_EQ(act5_waypoint->getAct(), 5);
}

TEST_F(WaypointTest, WaypointManagerInitialization) {
    EXPECT_GT(manager->getTotalCount(), 0);
    EXPECT_EQ(manager->getActivatedCount(), 0); // No waypoints activated initially
}

TEST_F(WaypointTest, WaypointManagerActivation) {
    EXPECT_FALSE(manager->isWaypointActivated(WaypointArea::ROGUE_ENCAMPMENT));
    
    bool activated = manager->activateWaypoint(WaypointArea::ROGUE_ENCAMPMENT);
    EXPECT_TRUE(activated);
    EXPECT_TRUE(manager->isWaypointActivated(WaypointArea::ROGUE_ENCAMPMENT));
    EXPECT_EQ(manager->getActivatedCount(), 1);
}

TEST_F(WaypointTest, WaypointManagerFastTravel) {
    // Initially can't travel anywhere
    EXPECT_FALSE(manager->canTravelTo(WaypointArea::ROGUE_ENCAMPMENT));
    EXPECT_FALSE(manager->canTravelTo(WaypointArea::COLD_PLAINS));
    
    // Activate waypoints
    manager->activateWaypoint(WaypointArea::ROGUE_ENCAMPMENT);
    manager->activateWaypoint(WaypointArea::COLD_PLAINS);
    
    // Now can travel to activated waypoints
    EXPECT_TRUE(manager->canTravelTo(WaypointArea::ROGUE_ENCAMPMENT));
    EXPECT_TRUE(manager->canTravelTo(WaypointArea::COLD_PLAINS));
    EXPECT_FALSE(manager->canTravelTo(WaypointArea::STONY_FIELD));
    
    // Travel to waypoint
    bool traveled = manager->travelTo(WaypointArea::COLD_PLAINS);
    EXPECT_TRUE(traveled);
}

TEST_F(WaypointTest, WaypointManagerGetByAct) {
    auto act1_waypoints = manager->getWaypointsByAct(1);
    auto act2_waypoints = manager->getWaypointsByAct(2);
    auto act5_waypoints = manager->getWaypointsByAct(5);
    
    EXPECT_GT(act1_waypoints.size(), 0);
    EXPECT_GT(act2_waypoints.size(), 0);
    EXPECT_GT(act5_waypoints.size(), 0);
    
    // Check that waypoints are in correct act
    for (const auto& waypoint : act1_waypoints) {
        EXPECT_EQ(waypoint->getAct(), 1);
    }
    
    for (const auto& waypoint : act2_waypoints) {
        EXPECT_EQ(waypoint->getAct(), 2);
    }
}

TEST_F(WaypointTest, WaypointSerialization) {
    // Activate some waypoints
    manager->activateWaypoint(WaypointArea::ROGUE_ENCAMPMENT);
    manager->activateWaypoint(WaypointArea::COLD_PLAINS);
    manager->activateWaypoint(WaypointArea::LUT_GHOLEIN);
    
    EXPECT_EQ(manager->getActivatedCount(), 3);
    
    // Get serialized data
    auto activated_ids = manager->getActivatedWaypointIds();
    EXPECT_EQ(activated_ids.size(), 3);
    
    // Create new manager and load data
    WaypointManager new_manager;
    new_manager.initializeDefaultWaypoints();
    new_manager.loadActivatedWaypoints(activated_ids);
    
    // Check that waypoints are activated
    EXPECT_EQ(new_manager.getActivatedCount(), 3);
    EXPECT_TRUE(new_manager.isWaypointActivated(WaypointArea::ROGUE_ENCAMPMENT));
    EXPECT_TRUE(new_manager.isWaypointActivated(WaypointArea::COLD_PLAINS));
    EXPECT_TRUE(new_manager.isWaypointActivated(WaypointArea::LUT_GHOLEIN));
}

TEST_F(WaypointTest, WaypointRetrieval) {
    auto waypoint = manager->getWaypoint(WaypointArea::ROGUE_ENCAMPMENT);
    EXPECT_NE(waypoint, nullptr);
    EXPECT_EQ(waypoint->getArea(), WaypointArea::ROGUE_ENCAMPMENT);
    
    auto nonexistent = manager->getWaypoint(WaypointArea::UNKNOWN);
    EXPECT_EQ(nonexistent, nullptr);
}

} // namespace d2::game