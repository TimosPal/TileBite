# Recursively find all relevant files in src directory
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/*.hpp)
file(GLOB_RECURSE ENGINE_TPP CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.tpp ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/*.tpp)

# Create GameEngine as a library
add_library(GameEngine ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_TPP})

# Enable precompiled header support
target_precompile_headers(GameEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/core/PCH.hpp)

# Set include directories
target_include_directories(GameEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include # For public API
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src # For internal engine code
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include # Add spdlog's include directory
)

# Add spdlog
add_subdirectory(external/spdlog)

# Link it to your target
target_link_libraries(GameEngine PUBLIC spdlog::spdlog)