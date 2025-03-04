cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(stdui VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True) 

set(OpenGL_GL_PREFERENCE GLVND)

# Define the parent directory (one level up from the cmake directory)
set(PARENT_DIR ${CMAKE_SOURCE_DIR}/..)

set(HEADERS
    ${PARENT_DIR}/widgets.h
    ${PARENT_DIR}/window.h
    ${PARENT_DIR}/internal/utils.h
    ${PARENT_DIR}/image.h
)

set(SOURCES
    ${PARENT_DIR}/internal/test.c
)

# Create the library target
add_library(stdui STATIC ${HEADERS} ${SOURCES})

# Include directories for project and dependencies
target_include_directories(stdui PUBLIC
    ${PARENT_DIR}/
)

# Link the appropriate libraries
target_link_libraries(stdui PRIVATE m GL X11 GLX)

install(TARGETS stdui
    DESTINATION /usr/local/lib
)
install(DIRECTORY ${PARENT_DIR}/
    DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)

