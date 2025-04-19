file(GLOB_RECURSE SRC_WORLD *.cpp)
add_library(world_feature STATIC ${SRC_WORLD})
target_link_libraries(world_feature PUBLIC core shared_utils)
target_include_directories(world_feature PUBLIC ${CMAKE_CURRENT_LIST_DIR}) 