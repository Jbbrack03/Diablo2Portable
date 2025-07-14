cmake_minimum_required(VERSION 3.22.1)
project(Diablo2Portable VERSION 1.5.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
    
    # Enable optimization in Release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /permissive-)
    
    # Enable optimization in Release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /DNDEBUG)
    endif()
endif()

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# Find packages
find_package(Threads REQUIRED)
find_package(BZip2 REQUIRED)

# AddressSanitizer support
if(ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# Add third-party libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(STORM_BUILD_TESTS OFF CACHE BOOL "Build StormLib tests" FORCE)
add_subdirectory(third_party/StormLib)

# Add subdirectories
add_subdirectory(engine)

# Tools subdirectory (optional) - disabled due to missing source files
# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tools/CMakeLists.txt)
#     add_subdirectory(tools)
# endif()

# Enable testing if requested
if(BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

