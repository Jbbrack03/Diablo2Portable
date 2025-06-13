cmake_minimum_required(VERSION 3.22.1)

# Test MPQ program
add_executable(test_mpq_real test_mpq_real.cpp)
target_link_libraries(test_mpq_real PRIVATE d2engine)
target_include_directories(test_mpq_real PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)