cmake_minimum_required(VERSION 3.22.1)

# Test MPQ program
add_executable(test_mpq_real test_mpq_real.cpp)
target_link_libraries(test_mpq_real PRIVATE d2engine)
target_include_directories(test_mpq_real PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Other tools
add_executable(test_compression test_compression.cpp)
target_link_libraries(test_compression PRIVATE d2engine)
target_include_directories(test_compression PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

add_executable(analyze_mpq analyze_mpq.cpp)
target_link_libraries(analyze_mpq PRIVATE d2engine)
target_include_directories(analyze_mpq PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

add_executable(try_known_files try_known_files.cpp)
target_link_libraries(try_known_files PRIVATE d2engine)
target_include_directories(try_known_files PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

add_executable(find_extractable find_extractable.cpp)
target_link_libraries(find_extractable PRIVATE d2engine)
target_include_directories(find_extractable PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

add_executable(check_compression check_compression.cpp)
target_link_libraries(check_compression PRIVATE d2engine)
target_include_directories(check_compression PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)