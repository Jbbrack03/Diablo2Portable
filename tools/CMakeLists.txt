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

add_executable(test_dc6_extraction test_dc6_extraction.cpp)
target_link_libraries(test_dc6_extraction PRIVATE d2engine)
target_include_directories(test_dc6_extraction PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test PRIVATE d2engine)
target_include_directories(integration_test PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# MPQ Extractor tool
add_executable(mpq_extractor mpq_extractor.cpp)
target_link_libraries(mpq_extractor PRIVATE d2engine)
target_include_directories(mpq_extractor PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Asset Cache Builder tool
add_executable(asset_cache_builder asset_cache_builder.cpp)
target_link_libraries(asset_cache_builder PRIVATE d2engine)
target_include_directories(asset_cache_builder PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Debug extraction tool
add_executable(debug_extraction debug_extraction.cpp)
target_link_libraries(debug_extraction PRIVATE d2engine)
target_include_directories(debug_extraction PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test PKWARE with real files
add_executable(test_pkware_real test_pkware_real.cpp)
target_link_libraries(test_pkware_real PRIVATE d2engine)
target_include_directories(test_pkware_real PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)