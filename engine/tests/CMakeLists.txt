cmake_minimum_required(VERSION 3.22.1)

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test sources
set(TEST_SOURCES
    main.cpp
    mpq/mpq_loader_test.cpp
    sprites/dc6_parser_test.cpp
    core/asset_manager_test.cpp
    rendering/egl_context_test.cpp
    rendering/renderer_test.cpp
    rendering/shader_manager_test.cpp
    rendering/texture_manager_test.cpp
    rendering/sprite_renderer_test.cpp
    input/gamepad_manager_test.cpp
    input/input_manager_test.cpp
    input/control_mapper_test.cpp
    game/character_test.cpp
    game/combat_test.cpp
    game/item_test.cpp
    game/skill_test.cpp
    game/monster_test.cpp
    # Tests will be added as we implement features following TDD
    # core/game_state_test.cpp
    # audio/sound_system_test.cpp
    # networking/network_manager_test.cpp
)

# Create test executable
add_executable(d2_unit_tests ${TEST_SOURCES})

# Link test executable with Google Test and engine library
target_link_libraries(d2_unit_tests
    PRIVATE
        d2engine
        gtest
        gmock
        gtest_main
)

# Include directories for tests
target_include_directories(d2_unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(d2_unit_tests)

# Copy test data if exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()