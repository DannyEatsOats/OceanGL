cmake_minimum_required(VERSION 3.28)
project(graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/GLAD)

file(GLOB_RECURSE SOURCE_DIR CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp
     ${CMAKE_SOURCE_DIR}/src/*.h)

add_executable(graphics ${SOURCE_DIR})

target_include_directories(
  graphics PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include
                  ${CMAKE_SOURCE_DIR}/vendor/GLAD/include
                  ${CMAKE_SOURCE_DIR}/vendor/glm/
                  )

target_link_libraries(graphics PRIVATE glfw glad)

target_compile_definitions(graphics
                           PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_custom_command(
  TARGET graphics
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
