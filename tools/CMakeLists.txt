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

# Debug PKWARE distance issues
add_executable(debug_pkware_distance debug_pkware_distance.cpp)
target_link_libraries(debug_pkware_distance PRIVATE d2engine)
target_include_directories(debug_pkware_distance PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test DC6 extraction manually
add_executable(test_dc6_manually test_dc6_manually.cpp)
target_link_libraries(test_dc6_manually PRIVATE d2engine)
target_include_directories(test_dc6_manually PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Check DC6 compression types
add_executable(check_dc6_compression check_dc6_compression.cpp)
target_link_libraries(check_dc6_compression PRIVATE d2engine)
target_include_directories(check_dc6_compression PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Analyze DC6 files
add_executable(analyze_dc6_files analyze_dc6_files.cpp)
target_link_libraries(analyze_dc6_files PRIVATE d2engine)
target_include_directories(analyze_dc6_files PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test single DC6 file (disabled due to compilation issues)
# add_executable(test_single_dc6 test_single_dc6.cpp)
# target_link_libraries(test_single_dc6 PRIVATE d2engine)
# target_include_directories(test_single_dc6 PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Dump DC6 bytes
add_executable(dump_dc6_bytes dump_dc6_bytes.cpp)
target_link_libraries(dump_dc6_bytes PRIVATE d2engine)
target_include_directories(dump_dc6_bytes PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Find DC6 header
add_executable(find_dc6_header find_dc6_header.cpp)
target_link_libraries(find_dc6_header PRIVATE d2engine)
target_include_directories(find_dc6_header PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test StormLib DC6 extraction
add_executable(test_stormlib_dc6 test_stormlib_dc6.cpp)
target_link_libraries(test_stormlib_dc6 PRIVATE d2engine)
target_include_directories(test_stormlib_dc6 PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# StormLib integration test
add_executable(stormlib_integration_test stormlib_integration_test.cpp)
target_link_libraries(stormlib_integration_test PRIVATE d2engine)
target_include_directories(stormlib_integration_test PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test AssetManager with StormLib
add_executable(test_asset_manager_stormlib test_asset_manager_stormlib.cpp)
target_link_libraries(test_asset_manager_stormlib PRIVATE d2engine)
target_include_directories(test_asset_manager_stormlib PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Test all file extraction
add_executable(test_all_extraction test_all_extraction.cpp)
target_link_libraries(test_all_extraction PRIVATE d2engine)
target_include_directories(test_all_extraction PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Extract from all MPQs
add_executable(extract_all_mpqs extract_all_mpqs.cpp)
target_link_libraries(extract_all_mpqs PRIVATE d2engine)
target_include_directories(extract_all_mpqs PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Diagnose MPQ file issues
add_executable(diagnose_mpq_issues diagnose_mpq_issues.cpp)
target_link_libraries(diagnose_mpq_issues PRIVATE d2engine)
target_include_directories(diagnose_mpq_issues PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)