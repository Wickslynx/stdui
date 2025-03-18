cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(stdui VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True) 

set(OpenGL_GL_PREFERENCE GLVND)

# Define the parent directory
set(PARENT_DIR ${CMAKE_SOURCE_DIR}/..)

# List of headers
set(HEADERS
    ${PARENT_DIR}/stdui/widgets.h
    ${PARENT_DIR}/stdui/window.h
    ${PARENT_DIR}/stdui/colors.h
    ${PARENT_DIR}/stdui/image.h
    ${PARENT_DIR}/stdui/internal/layout.h
    ${PARENT_DIR}/stdui/internal/font.h
    ${PARENT_DIR}/stdui/internal/arial.ttf
    ${PARENT_DIR}/stdui/internal/stb_truetype.h
    ${PARENT_DIR}/stdui/internal/stb_image.h
)

# List of sources
set(SOURCES
    ${PARENT_DIR}/stdui/internal/test.c
)

# Create the library target
add_library(stdui STATIC ${HEADERS} ${SOURCES})

# Include directories for project and dependencies
target_include_directories(stdui PUBLIC
    ${PARENT_DIR}/stdui/
)

# Link the appropriate libraries
target_link_libraries(stdui PRIVATE m GL X11 GLX)

install(TARGETS stdui
    DESTINATION /usr/local/lib
)

install(DIRECTORY ${PARENT_DIR}/stdui/
    DESTINATION /usr/local/include/stdui
    FILES_MATCHING PATTERN "*.h"
)
