add_library(shared_utils INTERFACE)

target_include_directories(shared_utils INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(shared_utils INTERFACE
    raylib
) 