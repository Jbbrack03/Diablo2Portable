cmake_minimum_required(VERSION 3.22.1)
project(Diablo2Portable VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)

# Find packages
find_package(Threads REQUIRED)
find_package(BZip2 REQUIRED)

# Add subdirectories
add_subdirectory(engine)

# Tools subdirectory (optional)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tools/CMakeLists.txt)
    add_subdirectory(tools)
endif()

# Enable testing if requested
if(BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

