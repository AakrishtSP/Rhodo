cmake_minimum_required(VERSION 3.30)

project(RhodoEngine
        VERSION 0.1.0
        DESCRIPTION "Modern C++23 Game Engine with Module Support"
        LANGUAGES C CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE and tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RH_ENABLE_CLANG_TIDY "Enable clang-tidy checks" ON)
option(RH_ENABLE_CLANG_FORMAT "Enable clang-format target" ON)

option(RH_ENABLE_DEBUG "Enable debug utilities" ON)
if (RH_ENABLE_DEBUG)
    message("Debug utilities enabled")
    add_compile_definitions(RH_ENABLE_DEBUG)
endif ()

option(RH_ENABLE_INSTRUMENTATION "Enable runtime instrumentation" OFF)

if (RH_ENABLE_INSTRUMENTATION)
    add_compile_definitions(RH_ENABLE_INSTRUMENTATION)
endif ()


# Add custom module paths
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# Set default build type if not specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()


# Platform detection
if (WIN32)
    set(RH_PLATFORM "Windows" CACHE INTERNAL "")
elseif (APPLE)
    set(RH_PLATFORM "Mac" CACHE INTERNAL "")
elseif (UNIX)
    set(RH_PLATFORM "Linux" CACHE INTERNAL "")
else ()
    set(RH_PLATFORM "Unknown" CACHE INTERNAL "")
endif ()

# Helper function for per-target output directories
function(set_target_output_dirs target_name)
    # Base directories for single-config generators
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/
    )

    # Per-configuration directories for multi-config generators
    foreach (OUTPUT_CONFIG Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG_UPPER)
        set_target_properties(${target_name} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                ${CMAKE_BINARY_DIR}/${OUTPUT_CONFIG}/bin/
                LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                ${CMAKE_BINARY_DIR}/${OUTPUT_CONFIG}/lib/
                ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                ${CMAKE_BINARY_DIR}/${OUTPUT_CONFIG}/lib/
        )
    endforeach ()
endfunction()

# Include code quality tools
include(cmake/ClangFormat.cmake)
include(cmake/CodeQuality.cmake)
include(cmake/ClangTidy.cmake)

# Add subdirectories (no project() calls in subdirectories)
add_subdirectory(engine)
add_subdirectory(application)

# Create combined fix target (nuclear button - runs tidy-fix + format)
if (RH_ENABLE_CLANG_TIDY AND RH_ENABLE_CLANG_FORMAT)
    add_custom_target(fix
            DEPENDS tidy-fix format
            COMMENT "Running full clang-tidy + clang-format pipeline"
    )
    message(STATUS "Created 'fix' target - runs tidy-fix and format in sequence")
elseif (RH_ENABLE_CLANG_TIDY)
    add_custom_target(fix
            DEPENDS tidy-fix
            COMMENT "Running clang-tidy fixes (format disabled)"
    )
elseif (RH_ENABLE_CLANG_FORMAT)
    add_custom_target(fix
            DEPENDS format
            COMMENT "Running clang-format (tidy disabled)"
    )
endif ()

include(cmake/Summary.cmake)
