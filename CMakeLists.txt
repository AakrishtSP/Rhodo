cmake_minimum_required(VERSION 3.31)
project(RhodoEngine LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent
include(FetchContent)

# Platform detection
if(WIN32)
    set(RH_PLATFORM "Windows" CACHE INTERNAL "")
elseif(APPLE)
    set(RH_PLATFORM "Mac" CACHE INTERNAL "")
elseif(UNIX)
    set(RH_PLATFORM "Linux" CACHE INTERNAL "")
else()
    set(RH_PLATFORM "Unknown" CACHE INTERNAL "")
endif()

# Helper function for per-target output directories
# Supports both single-config (Makefiles/Ninja) and multi-config (VS/Xcode) generators
function(set_target_output_dirs target_name)
    # Base directories for single-config generators
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${RH_PLATFORM}/${target_name}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/${RH_PLATFORM}/${target_name}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${RH_PLATFORM}/${target_name}
    )
    
    # Per-configuration directories for multi-config generators
    foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/build/bin/${RH_PLATFORM}/${target_name}/${OUTPUTCONFIG}
            LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/build/lib/${RH_PLATFORM}/${target_name}/${OUTPUTCONFIG}
            ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/build/obj/${RH_PLATFORM}/${target_name}/${OUTPUTCONFIG}
        )
    endforeach()
endfunction()

# Add subdirectories (no project() calls in subdirectories)
add_subdirectory(Rhodo)
add_subdirectory(Examples)

option(RH_BUILD_TESTS "Build unit tests" ON)

if(RH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()