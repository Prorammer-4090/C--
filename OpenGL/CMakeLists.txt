cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Try to find Assimp
find_package(assimp QUIET)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/includes/glad)
include_directories(${CMAKE_SOURCE_DIR}/includes/glm)
include_directories(/opt/homebrew/include)

# Add all source files
file(GLOB SOURCES "*.cpp")
set(GLAD_SOURCES "${CMAKE_SOURCE_DIR}/includes/glad/src/glad.c")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLAD_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    glfw
)

# Link Assimp
if(assimp_FOUND)
    target_link_libraries(${PROJECT_NAME} assimp::assimp)
else()
    # Fallback to manual linking if find_package fails
    target_link_directories(${PROJECT_NAME} PRIVATE 
        /opt/homebrew/lib
        ${CMAKE_SOURCE_DIR}/includes/assimp/lib
    )
    target_link_libraries(${PROJECT_NAME} assimp)
endif()

# Link system frameworks (macOS)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} 
        "-framework OpenGL"
        "-framework Cocoa" 
        "-framework IOKit"
    )
endif()

# Optional: Link SDL2 if you're using it
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SDL2 QUIET sdl2)
    if(SDL2_FOUND)
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    endif()
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output
)
