cmake_minimum_required(VERSION 3.22.1)

# Engine library sources
set(ENGINE_SOURCES
    # Core will be added as we implement
    # src/core/game.cpp
    # src/rendering/renderer.cpp
    # src/input/controller.cpp
    # src/audio/sound_system.cpp
    # src/networking/network_manager.cpp
    # src/utils/mpq_loader.cpp
)

# Engine headers
set(ENGINE_HEADERS
    # Headers will be added as we implement
)

# Create engine library (currently empty, will be populated)
add_library(d2engine STATIC 
    src/dummy.cpp
    src/utils/stormlib_mpq_loader.cpp
    src/utils/pkware_explode.cpp
    src/utils/huffman_decompress.cpp
    src/utils/bzip2_decompress.cpp
    src/utils/sparse_decompress.cpp
    src/utils/mock_mpq_builder.cpp
    src/utils/security_utils.cpp
    src/utils/file_utils.cpp
    src/utils/validation_framework.cpp
    src/utils/mpq_validator.cpp
    src/utils/data_table_parser.cpp
    src/core/resource_manager.cpp
    src/sprites/dc6_parser.cpp
    src/core/asset_manager.cpp
    src/core/settings_manager.cpp
    src/rendering/egl_context.cpp
    src/rendering/renderer.cpp
    src/rendering/shader_manager.cpp
    src/rendering/texture_manager.cpp
    src/rendering/sprite_renderer.cpp
    src/rendering/vertex_buffer.cpp
    src/rendering/vertex_buffer_pool.cpp
    src/rendering/vertex_array_object.cpp
    src/rendering/mock_opengl.cpp
    src/rendering/world_renderer.cpp
    src/rendering/optimized_world_renderer.cpp
    src/rendering/camera.cpp
    src/rendering/sprite_animation.cpp
    src/animation/animation_controller.cpp
    src/input/gamepad_manager.cpp
    src/input/input_manager.cpp
    src/input/control_mapper.cpp
    src/input/touch_input.cpp
    src/input/android_input_impl.cpp
    src/input/android_gamepad.cpp
    src/game/character.cpp
    src/game/combat_engine.cpp
    src/game/item.cpp
    src/game/skill.cpp
    src/game/skill_tree.cpp
    src/game/monster.cpp
    src/game/inventory.cpp
    src/game/character_inventory.cpp
    src/game/loot_system.cpp
    src/game/player.cpp
    src/game/game_state.cpp
    src/game/game_engine.cpp
    src/game/entity.cpp
    src/game/entity_manager.cpp
    src/game/quest.cpp
    src/game/quest_manager.cpp
    src/game/item_database.cpp
    src/game/npc.cpp
    src/game/waypoint.cpp
    src/map/map_loader.cpp
    src/map/pathfinder.cpp
    src/map/ds1_parser.cpp
    src/world/collision_system.cpp
    src/world/collision_response.cpp
    src/world/world_object.cpp
    src/world/interaction_manager.cpp
    src/audio/audio_engine.cpp
    src/network/network_manager.cpp
    src/network/game_session.cpp
    src/network/network_game.cpp
    src/ui/ui_element.cpp
    src/ui/ui_button.cpp
    src/ui/ui_panel.cpp
    src/ui/ui_manager.cpp
    src/ui/inventory_screen.cpp
    src/ui/character_screen.cpp
    src/ui/skill_screen.cpp
    src/ui/main_menu_screen.cpp
    src/ui/pause_screen.cpp
    src/ui/ui_renderer.cpp
    src/ui/ui_layout.cpp
    src/ui/font.cpp
    src/ui/font_manager.cpp
    src/ui/text_renderer.cpp
    src/ui/tutorial_system.cpp
    src/ui/help_system.cpp
    src/ui/asset_management_ui.cpp
    src/ui/extraction_wizard_ui.cpp
    src/ui/extraction_help_dialog.cpp
    src/ui/menu_screen.cpp
    src/save/save_manager.cpp
    src/android/jni_bridge.cpp
    src/android/gamepad_handler.cpp
    src/android/apk_asset_loader.cpp
    src/android/asset_path_validator.cpp
    src/performance/performance_monitor.cpp
    src/performance/optimized_update_system.cpp
    src/performance/memory_monitor.cpp
    src/tools/asset_extractor.cpp
    src/tools/extraction_monitor.cpp
    src/tools/differential_extractor.cpp
    src/tools/asset_cache.cpp
    src/tools/multi_format_processor.cpp
    src/tools/asset_verifier.cpp
    src/tools/asset_optimizer.cpp
    src/tools/texture_atlas_generator.cpp
    src/tools/asset_manifest.cpp
    src/tools/apk_packager.cpp
    src/tools/asset_browser_backend.cpp
    src/onboarding/file_source_detector.cpp
    src/onboarding/onboarding_wizard.cpp
    src/onboarding/onboarding_jni.cpp
    src/onboarding/file_format_handler.cpp
    src/onboarding/asset_validator.cpp
    src/extraction/iso_extractor.cpp
    src/extraction/patch_system.cpp
    src/extraction/extraction_coordinator.cpp
)

# Include directories
target_include_directories(d2engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile options
target_compile_options(d2engine PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Find required packages
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Fetch GLM for mathematics
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# Link libraries
target_link_libraries(d2engine
    PUBLIC
        Threads::Threads
        glm::glm
        ZLIB::ZLIB
        BZip2::BZip2
        storm
)

# Platform-specific settings
if(ANDROID)
    target_link_libraries(d2engine PUBLIC
        log
        android
        EGL
        GLESv3
    )
endif()

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()