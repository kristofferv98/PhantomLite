# UI Slice CMakeLists.txt

file(GLOB_RECURSE SRC_UI *.cpp)
add_library(ui STATIC ${SRC_UI})
target_link_libraries(ui PUBLIC core fmt::fmt)
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Link with dependencies (core for types, fmt for UI text formatting)
target_link_libraries(ui
    PUBLIC raylib
)

# Add tests
add_executable(test_ui
    tests/test_hearts.cpp
)

target_link_libraries(test_ui
    PRIVATE ui
    PRIVATE Catch2::Catch2
)

# Register with CTest
add_test(NAME test_ui COMMAND test_ui) 