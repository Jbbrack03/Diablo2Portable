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
    src/utils/mpq_loader.cpp
    src/sprites/dc6_parser.cpp
    src/core/asset_manager.cpp
    src/rendering/egl_context.cpp
    src/rendering/renderer.cpp
    src/rendering/shader_manager.cpp
    src/rendering/texture_manager.cpp
    src/rendering/sprite_renderer.cpp
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