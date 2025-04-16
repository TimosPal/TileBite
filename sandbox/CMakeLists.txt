# Recursively find all .cpp and .hpp files in src directory
file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SANDBOX_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Make sure the engine is added before building the app
add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

# Link the app with the GameEngine library
target_link_libraries(Sandbox GameEngine)

# Include the engine headers
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
)