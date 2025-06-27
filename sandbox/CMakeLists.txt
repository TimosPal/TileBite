function(add_game_demo DEMO_NAME DEMO_SRC)
    add_executable(${DEMO_NAME} ${DEMO_SRC})
    target_link_libraries(${DEMO_NAME} GameEngine)
    target_include_directories(${DEMO_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)
endfunction()

add_game_demo(Sandbox       ${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox.cpp)
add_game_demo(SnakeDemo     ${CMAKE_CURRENT_SOURCE_DIR}/src/snakeDemo.cpp)
add_game_demo(TilemapDemo   ${CMAKE_CURRENT_SOURCE_DIR}/src/tilemapDemo.cpp)