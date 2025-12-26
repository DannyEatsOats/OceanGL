cmake_minimum_required(VERSION 3.16)
project(ImGui)

# Create ImGui static library with explicit source list
add_library(imgui STATIC
        imgui.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui_demo.cpp
)

# Include directory for ImGui
target_include_directories(imgui
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard to C++17
target_compile_features(imgui PUBLIC cxx_std_17)

# Disable static runtime linking if needed (equivalent to staticruntime "off" in Premake)
# In CMake this is typically handled globally or via toolchain, but for MSVC:
if (MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
        if(${flag_var} MATCHES "/MT")
            string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Platform-specific settings
if(WIN32)
    # Windows specific
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_WIN32_ENABLE)
elseif(UNIX)
    # Linux specific
    set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Output directories (matches your Premake bin/bin-int structure)
set_target_properties(imgui PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/ImGui
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/ImGui
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/ImGui
)

# Configuration-specific compile options (Debug/Release/Dist)
target_compile_definitions(imgui
        PRIVATE
        $<$<CONFIG:Debug>:IMGUI_DEBUG>
        $<$<CONFIG:Release>:IMGUI_RELEASE>
        $<$<CONFIG:Dist>:IMGUI_DIST>
)