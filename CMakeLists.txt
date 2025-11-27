cmake_minimum_required(VERSION 3.30)

project(RhodoEngine LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/")

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
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${target_name}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${target_name}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj/${target_name}
    )
    
    # Per-configuration directories for multi-config generators
    foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/bin/${target_name}
            LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib/${target_name}
            ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} 
                ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/obj/${target_name}
        )
    endforeach()
endfunction()

# Add subdirectories (no project() calls in subdirectories)
add_subdirectory(Rhodo)
add_subdirectory(Application)

option(RH_BUILD_TESTS "Build unit tests" ON)

if(RH_BUILD_TESTS)
    message("Building unit tests...")
    enable_testing()
    add_subdirectory(Tests)
endif()