# Examples Application

# Collect source files
file(GLOB_RECURSE EXAMPLES_SOURCES
    src/*.cpp
    src/*.h
)

# Create executable
add_executable(Examples ${EXAMPLES_SOURCES})

# Include directories
target_include_directories(Examples
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
#        ${CMAKE_ROOT}/Rhodo/include
)

# Compile definitions
target_compile_definitions(Examples
    PRIVATE 
        RH_IMPORT_STATIC
)

# Link Rhodo library (automatically inherits PUBLIC includes and dependencies)
target_link_libraries(Examples
    PRIVATE 
        Rhodo
)

# Set output directories
set_target_output_dirs(Examples)

# === HYBRID ASSET MANAGEMENT ===
# Development: Working directory points to source (live editing)
# Production: Assets copied to build directory (bundled)

# Set working directory to source folder for development (live asset editing)
set_target_properties(Examples PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Copy assets for production builds (Release/RelWithDebInfo only)
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
if(EXISTS ${ASSETS_DIR})
    # Generator expression: only run copy command for Release-type builds
    add_custom_command(TARGET Examples POST_BUILD
        COMMAND $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:${CMAKE_COMMAND}>
        ARGS -E copy_directory 
            ${ASSETS_DIR}
            $<TARGET_FILE_DIR:Examples>/assets
        COMMENT "Bundling assets for production build..."
    )
    message(STATUS "Assets found at: ${ASSETS_DIR}")
    message(STATUS "  - Debug builds: Live assets (no copy)")
    message(STATUS "  - Release builds: Assets will be bundled")
else()
    message(WARNING "Assets directory not found at: ${ASSETS_DIR}")
endif()
