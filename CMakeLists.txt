cmake_minimum_required (VERSION 3.8)

project ("GameEngine")

# Set C++ standard globally for the whole project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

add_subdirectory(engine)
add_subdirectory(sandbox)