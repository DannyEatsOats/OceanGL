cmake_minimum_required(VERSION 3.16)

# Create glad as a static library
add_library(glad STATIC
        src/glad.c
)

set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Include glad headers publicly
target_include_directories(glad
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
