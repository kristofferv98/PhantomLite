# UI Slice - Health, Inventory, Dialog components

add_library(ui 
    ui.cpp
    atoms/hearts.cpp
    molecules/hearts_controller.cpp
)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link against dependencies
target_link_libraries(ui PUBLIC raylib)

# Add tests
add_executable(test_ui_hearts tests/test_hearts.cpp atoms/hearts.cpp)
target_link_libraries(test_ui_hearts PRIVATE Catch2::Catch2 raylib)
add_test(NAME test_ui_hearts COMMAND test_ui_hearts) 