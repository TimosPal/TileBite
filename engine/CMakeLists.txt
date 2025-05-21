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

# Platform flags
# All platforms use OpenGL for now.
if(WIN32)
    target_compile_definitions(GameEngine PRIVATE RENDERER_OPENGL)
    target_compile_definitions(GameEngine PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(GameEngine PRIVATE RENDERER_OPENGL)
    target_compile_definitions(GameEngine PRIVATE PLATFORM_MACOS)
elseif(UNIX)
    target_compile_definitions(GameEngine PRIVATE RENDERER_OPENGL)
    target_compile_definitions(GameEngine PRIVATE PLATFORM_LINUX)
endif()

# Choose one backend
option(USE_GLFW "Use GLFW as window backend" ON)
if(USE_GLFW)
    target_compile_definitions(GameEngine PRIVATE WINDOW_BACKEND_GLFW)
    
    add_subdirectory(external/glfw)
    target_link_libraries(GameEngine PRIVATE glfw)
    # Remove GLFW + OpenGL include order restriction
    target_compile_definitions(GameEngine PRIVATE GLFW_INCLUDE_NONE)
else()
    target_compile_definitions(GameEngine PRIVATE WINDOW_BACKEND_NATIVE)
    # Optionally add native-specific libs here
endif()

# Add OpenGL
# TODO: Add options for rendering API in the future.
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    add_library(glad STATIC external/glad/src/glad.c)
    target_include_directories(glad PUBLIC external/glad/include)
    target_link_libraries(GameEngine PUBLIC glad OpenGL::GL)
else()
    message(FATAL_ERROR "OpenGL not found")
endif()

# Add spdlog
add_subdirectory(external/spdlog)
target_link_libraries(GameEngine PUBLIC spdlog::spdlog)

# stb image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)
target_link_libraries(GameEngine PUBLIC stb_image)

target_compile_definitions(GameEngine PUBLIC ENGINE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets")