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

# List palette files
add_executable(list_pal_files list_pal_files.cpp)
target_link_libraries(list_pal_files PRIVATE d2engine)
target_include_directories(list_pal_files PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Diagnose MPQ issues
add_executable(diagnose_mpq diagnose_mpq.cpp)
target_link_libraries(diagnose_mpq PRIVATE d2engine)
target_include_directories(diagnose_mpq PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Extract listfile
add_executable(extract_listfile extract_listfile.cpp)
target_link_libraries(extract_listfile PRIVATE d2engine)
target_include_directories(extract_listfile PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Debug listfile
add_executable(debug_listfile debug_listfile.cpp)
target_link_libraries(debug_listfile PRIVATE d2engine)
target_include_directories(debug_listfile PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test one PKWARE file
add_executable(test_one_pkware test_one_pkware.cpp)
target_link_libraries(test_one_pkware PRIVATE d2engine)
target_include_directories(test_one_pkware PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Debug compression mask
add_executable(debug_compression_mask debug_compression_mask.cpp)
target_link_libraries(debug_compression_mask PRIVATE d2engine)
target_include_directories(debug_compression_mask PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Analyze MPQ compression
add_executable(analyze_mpq_compression analyze_mpq_compression.cpp)
target_link_libraries(analyze_mpq_compression PRIVATE d2engine)
target_include_directories(analyze_mpq_compression PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Debug sector offsets
add_executable(debug_sector_offsets debug_sector_offsets.cpp)
target_link_libraries(debug_sector_offsets PRIVATE d2engine)
target_include_directories(debug_sector_offsets PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Debug PKWARE bytes
add_executable(debug_pkware_bytes debug_pkware_bytes.cpp)
target_link_libraries(debug_pkware_bytes PRIVATE d2engine)
target_include_directories(debug_pkware_bytes PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)