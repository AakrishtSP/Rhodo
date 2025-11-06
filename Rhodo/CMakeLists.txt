# Rhodo Engine Library

# Collect source files
file(GLOB_RECURSE RHODO_SOURCES
        src/*.cpp
        src/*.h
        include/*.h
)

# Create static library
add_library(Rhodo STATIC
        ${RHODO_SOURCES}
)

# --------------------------------------------------------------------------
# Fetch spdlog (latest stable)
# --------------------------------------------------------------------------
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.16.0  # replace with desired version
)

FetchContent_MakeAvailable(spdlog)

# Add external dependencies
add_subdirectory(external/glm glm_build)
add_subdirectory(external/glfw glfw_build)

# Add Glad2
add_subdirectory(external/glad/cmake glad_build)
glad_add_library(glad_gl STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# Include directories
target_include_directories(Rhodo
        PUBLIC
        # For consumers of the library (Examples, etc.)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        # Internal implementation details
        /src
        ${CMAKE_CURRENT_SOURCE_DIR}/src
#        ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
        ${CMAKE_BINARY_DIR}/Rhodo/gladsources/glad_gl/include
)

# Precompiled headers
target_precompile_headers(Rhodo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/rhpch.h"
)

# Link libraries
target_link_libraries(Rhodo
        PRIVATE
        glfw
        glad_gl
        spdlog::spdlog
        PUBLIC
        glm::glm
)

# Build configuration defines (works with both single-config and multi-config generators)
target_compile_definitions(Rhodo
        PRIVATE
        RH_BUILD_STATIC


        # Debug configuration
        $<$<CONFIG:Debug>:RH_DEBUG;RH_ENABLE_LOGGING;RH_ENABLE_ASSERTS;RH_ENABLE_EVENT_NAMES>
        # Release with debug info configuration
        $<$<CONFIG:RelWithDebInfo>:RH_RELEASE;RH_ENABLE_LOGGING;RH_ENABLE_EVENT_NAMES>
        # Release configuration
        $<$<CONFIG:Release>:RH_RELEASE>
        # MinSizeRel configuration
        $<$<CONFIG:MinSizeRel>:RH_RELEASE>
)

# Set output directories
set_target_output_dirs(Rhodo)