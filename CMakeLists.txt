cmake_minimum_required (VERSION 3.8)

# Set C++ standard globally for the whole project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

# Add Debug-specific macro
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
endif()

add_subdirectory(engine)
add_subdirectory(sandbox)