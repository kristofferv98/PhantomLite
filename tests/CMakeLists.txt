set(TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/features/player/tests/test_player.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE player_feature Catch2::Catch2)
add_test(NAME all_tests COMMAND tests) 