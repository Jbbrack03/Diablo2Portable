#include <gtest/gtest.h>
#include "game/save_manager.h"

using namespace d2::game;

// Phase 20: Save System - Following strict TDD
// TEST 1: Can we create a SaveManager?
TEST(SaveManagerTest, CreateSaveManager) {
    SaveManager saveManager;
    // A save manager should exist - that's all we test for now
}