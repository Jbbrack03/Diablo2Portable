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
    mpq/test_file_encryption.cpp
    sprites/dc6_parser_test.cpp
    core/asset_manager_test.cpp
    core/test_asset_manager_mpq.cpp
    rendering/egl_context_test.cpp
    rendering/renderer_test.cpp
    rendering/shader_manager_test.cpp
    rendering/texture_manager_test.cpp
    rendering/sprite_renderer_test.cpp
    input/gamepad_manager_test.cpp
    input/input_manager_test.cpp
    input/control_mapper_test.cpp
    game/character_test.cpp
    game/character_inventory_test.cpp
    game/combat_test.cpp
    game/item_test.cpp
    game/skill_test.cpp
    game/monster_test.cpp
    game/inventory_test.cpp
    game/loot_system_test.cpp
    map/map_loader_test.cpp
    map/pathfinding_test.cpp
    map/test_ds1_parser.cpp
    world/test_collision_system.cpp
    world/test_entity_collision.cpp
    world/test_collision_response.cpp
    world/test_world_objects.cpp
    world/interaction_manager_test.cpp
    utils/pkware_test.cpp
    utils/huffman_decompress_test.cpp
    audio/test_audio_engine.cpp
    network/test_network_manager.cpp
    network/game_session_test.cpp
    utils/bzip2_test.cpp
    integration/test_real_mpq_files.cpp
    test_pkware_real_format.cpp
    ui/test_ui_element.cpp
    ui/test_ui_button.cpp
    ui/test_ui_panel.cpp
    ui/test_ui_label.cpp
    ui/test_font.cpp
    ui/test_font_manager.cpp
    ui/test_text_renderer.cpp
    ui/test_ui_manager.cpp
    ui/test_inventory_screen.cpp
    ui/test_character_screen.cpp
    ui/test_skill_screen.cpp
    ui/test_main_menu_screen.cpp
    ui/test_pause_screen.cpp
    ui/test_ui_renderer.cpp
    ui/test_ui_layout.cpp
    save/test_save_manager.cpp
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