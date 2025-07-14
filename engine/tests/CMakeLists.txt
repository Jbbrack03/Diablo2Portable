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
    mpq/stormlib_mpq_loader_test.cpp
    mpq/test_file_encryption.cpp
    mpq/test_stormlib_direct.cpp
    mpq/test_stormlib_stack_fix.cpp
    mpq/test_stormlib_thread_stack.cpp
    mpq/test_empty_mpq_detection.cpp
    sprites/dc6_parser_test.cpp
    core/asset_manager_test.cpp
    core/test_asset_manager_mpq.cpp
    core/test_asset_manager_mpq_fix.cpp
    core/asset_manager_memory_test.cpp
    core/settings_manager_test.cpp
    rendering/egl_context_test.cpp
    rendering/renderer_test.cpp
    rendering/opengl_implementation_test.cpp
    rendering/game_loop_update_test.cpp
    rendering/shader_manager_test.cpp
    rendering/shader_implementation_test.cpp
    rendering/real_opengl_shader_compilation_test.cpp
    rendering/texture_manager_test.cpp
    rendering/real_opengl_texture_operations_test.cpp
    rendering/real_opengl_vbo_test.cpp
    rendering/real_opengl_draw_commands_test.cpp
    rendering/real_opengl_draw_elements_test.cpp
    rendering/real_opengl_draw_elements_parameters_test.cpp
    rendering/sprite_renderer_test.cpp
    rendering/sprite_renderer_opengl_test.cpp
    rendering/opengl_draw_calls_test.cpp
    rendering/vertex_buffer_test.cpp
    rendering/opengl_vertex_buffer_test.cpp
    rendering/opengl_calls_required_test.cpp
    rendering/vertex_array_object_test.cpp
    rendering/shader_uniform_test.cpp
    rendering/opengl_draw_commands_test.cpp
    rendering/opengl_draw_implementation_test.cpp
    rendering/opengl_draw_arrays_test.cpp
    rendering/vertex_attribute_setup_test.cpp
    rendering/asset_texture_integration_test.cpp
    rendering/sprite_to_texture_test.cpp
    rendering/texture_binding_test.cpp
    rendering/alpha_blending_test.cpp
    rendering/depth_testing_test.cpp
    rendering/alpha_testing_test.cpp
    rendering/texture_clamping_test.cpp
    rendering/sprite_animation_test.cpp
    rendering/world_renderer_test.cpp
    rendering/camera_test.cpp
    input/gamepad_manager_test.cpp
    input/input_manager_test.cpp
    input/control_mapper_test.cpp
    input/test_touch_input.cpp
    input/android_input_test.cpp
    input/android_gamepad_test.cpp
    game/character_test.cpp
    game/character_inventory_test.cpp
    game/combat_test.cpp
    game/item_test.cpp
    game/skill_test.cpp
    game/monster_test.cpp
    game/inventory_test.cpp
    game/loot_system_test.cpp
    game/player_test.cpp
    game/game_state_test.cpp
    game/game_engine_input_test.cpp
    game/game_engine_render_test.cpp
    game/test_game_engine_touch.cpp
    game/entity_manager_test.cpp
    game/item_drop_integration_test.cpp
    game/quest_system_test.cpp
    test_game_engine.cpp
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
    utils/sparse_decompress_test.cpp
    utils/mpq_validator_test.cpp
    utils/mpq_validator_vendor_test.cpp
    test_pkware_explode_refactor.cpp
    test_file_utils.cpp
    test_menu_screen_refactor.cpp
    audio/test_audio_engine.cpp
    audio/audio_playback_test.cpp
    network/test_network_manager.cpp
    network/game_session_test.cpp
    network/network_game_test.cpp
    network/network_receive_test.cpp
    utils/bzip2_test.cpp
    integration/test_real_mpq_files.cpp
    integration/gameplay_integration_test.cpp
    integration/end_to_end_test.cpp
    integration/test_empty_mpq_handling.cpp
    integration/test_mpq_file_validation.cpp
    test_pkware_real_format.cpp
    ui/test_ui_element.cpp
    ui/test_ui_button.cpp
    ui/test_ui_panel.cpp
    ui/test_ui_label.cpp
    ui/test_font.cpp
    ui/test_font_real_implementation.cpp
    ui/test_font_manager.cpp
    ui/test_font_manager_real_implementation.cpp
    ui/test_text_renderer.cpp
    ui/test_text_renderer_opengl.cpp
    ui/test_ui_manager.cpp
    ui/test_inventory_screen.cpp
    ui/test_character_screen.cpp
    ui/test_skill_screen.cpp
    ui/test_main_menu_screen.cpp
    ui/test_pause_screen.cpp
    ui/test_ui_renderer.cpp
    ui/test_ui_layout.cpp
    ui/tutorial_system_test.cpp
    ui/help_system_test.cpp
    ui/accessibility_manager_test.cpp
    ui/input_remapper_test.cpp
    ui/asset_management_ui_test.cpp
    ui/extraction_wizard_ui_test.cpp
    ui/extraction_help_dialog_test.cpp
    save/test_save_manager.cpp
    android/test_android_structure.cpp
    android/test_jni_bridge.cpp
    android/test_android_jni_integration.cpp
    android/test_android_activity.cpp
    android/test_android_assets.cpp
    # android/test_android_gamepad.cpp  # DISABLED: Causes segfault/hang - tracked issue
    android/test_gamepad_jni.cpp
    android/test_apk_asset_loader.cpp
    android/android_build_validator_test.cpp
    android/android_sdk_setup_test.cpp
    android/android_apk_build_test.cpp
    android/android_jni_bridge_validation_test.cpp
    android/android_integration_test.cpp
    android/test_asset_path_integration.cpp
    android/test_java_asset_integration.cpp
    android/android_performance_validation_test.cpp
    android/android_opengl_performance_test.cpp
    performance/test_performance_metrics.cpp
    performance/test_game_performance.cpp
    performance/memory_monitor_test.cpp
    performance/memory_profiling_test.cpp
    performance/test_performance_monitor.cpp
    performance/test_optimized_update_system.cpp
    performance/performance_validation_test.cpp
    rendering/test_optimized_world_renderer.cpp
    tools/asset_extractor_test.cpp
    tools/asset_optimizer_test.cpp
    tools/texture_atlas_test.cpp
    tools/asset_manifest_test.cpp
    tools/apk_packager_test.cpp
    tools/extraction_monitor_test.cpp
    tools/differential_extractor_test.cpp
    tools/differential_extractor_proper_test.cpp
    tools/asset_cache_test.cpp
    tools/multi_format_processor_test.cpp
    tools/asset_verifier_test.cpp
    tools/asset_browser_backend_test.cpp
    device/test_device_compatibility.cpp
    device/test_device_tester.cpp
    device/device_compatibility_validation_test.cpp
    release/test_release_builder.cpp
    onboarding/file_source_detector_test.cpp
    onboarding/onboarding_wizard_test.cpp
    onboarding/onboarding_jni_test.cpp
    onboarding/file_format_handler_test.cpp
    onboarding/asset_validator_test.cpp
    onboarding/extraction_options_test.cpp
    security/security_privacy_test.cpp
    extraction/test_iso_extractor.cpp
    extraction/test_patch_system.cpp
    extraction/test_extraction_coordinator.cpp
    integration/gameplay_integration_test.cpp
    integration/end_to_end_test.cpp
    integration/test_real_mpq_files.cpp
    integration/test_empty_mpq_handling.cpp
    integration/test_mpq_file_validation.cpp
    integration/test_mock_mpq_integration.cpp
    integration/critical_systems_validation_test.cpp
    integration/asset_pipeline_repair_test.cpp
    integration/production_readiness_test.cpp
    integration/mpq_integration_test.cpp
    meta/test_count_validator_test.cpp
    meta/phase_completion_validator_test.cpp
    meta/phase_audit_test.cpp
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