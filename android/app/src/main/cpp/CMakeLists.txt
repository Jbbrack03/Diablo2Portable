cmake_minimum_required(VERSION 3.22.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Define the library name
set(LIB_NAME d2engine)

# Get all engine source files
file(GLOB_RECURSE ENGINE_SOURCES
    "../../../../engine/src/*.cpp"
)

# Remove any files that might not be suitable for Android builds
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*dummy\\.cpp$")

# Create the shared library for Android
add_library(d2engine SHARED ${ENGINE_SOURCES})

# Include directories
target_include_directories(${LIB_NAME} PRIVATE
    ../../../../engine/include
    ../../../../engine/src
    ../../../../third_party/StormLib/src
)

# Find required Android libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

# Link Android-specific libraries
target_link_libraries(${LIB_NAME}
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
)

# Add StormLib as a subdirectory (if available)
if(EXISTS "../../../../third_party/StormLib/CMakeLists.txt")
    add_subdirectory(../../../../third_party/StormLib ${CMAKE_BINARY_DIR}/stormlib)
    target_link_libraries(${LIB_NAME} storm)
endif()

# Find and link additional libraries
find_package(Threads REQUIRED)
target_link_libraries(${LIB_NAME} Threads::Threads)

# GLM for mathematics
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)
target_link_libraries(${LIB_NAME} glm::glm)

# Compiler flags
target_compile_options(${LIB_NAME} PRIVATE
    -Wall -Wextra
    -fvisibility=hidden
    -ffunction-sections
    -fdata-sections
)

# Linker flags
target_link_options(${LIB_NAME} PRIVATE
    -Wl,--gc-sections
    -Wl,--strip-all
)