file(GLOB SRC_PLAYER *.cpp)
add_library(player_feature STATIC ${SRC_PLAYER})
target_link_libraries(player_feature PUBLIC core)
target_include_directories(player_feature PUBLIC ${CMAKE_CURRENT_LIST_DIR}) 