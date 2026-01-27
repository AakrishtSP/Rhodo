# CodeQuality.cmake - Unified Code Quality Helper Functions

# Helper to gather all project sources
function(rh_collect_sources out_var)
    file(GLOB_RECURSE SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/application/*.cpp"
        "${CMAKE_SOURCE_DIR}/application/*.cppm"
        "${CMAKE_SOURCE_DIR}/application/*.ixx"
        "${CMAKE_SOURCE_DIR}/application/*.hpp"
        "${CMAKE_SOURCE_DIR}/application/*.h"
        "${CMAKE_SOURCE_DIR}/engine/*.cpp"
        "${CMAKE_SOURCE_DIR}/engine/*.cppm"
        "${CMAKE_SOURCE_DIR}/engine/*.ixx"
        "${CMAKE_SOURCE_DIR}/engine/*.hpp"
        "${CMAKE_SOURCE_DIR}/engine/*.h"
        "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    )
    set(${out_var} ${SOURCES} PARENT_SCOPE)
endfunction()

# Function to register a target for code quality tools
function(register_code_quality target_name)
    if (NOT TARGET ${target_name})
        return()
    endif ()

    # Enable clang-tidy if available
    if (COMMAND rh_enable_clang_tidy)
        rh_enable_clang_tidy(${target_name})
    endif ()
endfunction()

# Summary
message(STATUS "CodeQuality helper module loaded")
