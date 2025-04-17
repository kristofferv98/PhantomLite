file(GLOB_RECURSE TEST_SOURCES ../src/*/tests/*.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE player_feature Catch2::Catch2WithMain)
add_test(NAME all_tests COMMAND tests) 