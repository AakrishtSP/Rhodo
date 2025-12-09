# CodeQuality.cmake - Unified Code Quality Helper Functions

# ============================================================================
# Shared source collection function used by both format and tidy
# ============================================================================

# Collects all C++ source files from a target including:
# - Regular SOURCES
# - INTERFACE_SOURCES
# - CXX_MODULES file sets
# Returns absolute paths to source files
# Args:
#   target_name - Name of the CMake target
#   out_var - Output variable name for the list of source files
function(collect_target_sources target_name out_var)
    if (NOT TARGET ${target_name})
        message(DEBUG "collect_target_sources: Target '${target_name}' does not exist")
        set(${out_var} "" PARENT_SCOPE)
        return()
    endif ()

    set(ALL_SOURCES "")

    # Get regular sources
    get_target_property(TARGET_SOURCES ${target_name} SOURCES)
    if (TARGET_SOURCES AND NOT TARGET_SOURCES STREQUAL "TARGET_SOURCES-NOTFOUND")
        list(APPEND ALL_SOURCES ${TARGET_SOURCES})
    endif ()

    # Get interface sources
    get_target_property(INTERFACE_SOURCES ${target_name} INTERFACE_SOURCES)
    if (INTERFACE_SOURCES AND NOT INTERFACE_SOURCES STREQUAL "INTERFACE_SOURCES-NOTFOUND")
        list(APPEND ALL_SOURCES ${INTERFACE_SOURCES})
    endif ()

    # Get CXX_MODULES file set sources (C++20 modules)
    get_target_property(HAS_CXX_MODULES ${target_name} CXX_MODULE_SETS)
    if (HAS_CXX_MODULES AND NOT HAS_CXX_MODULES STREQUAL "HAS_CXX_MODULES-NOTFOUND")
        foreach (MODULE_SET ${HAS_CXX_MODULES})
            get_target_property(MODULE_SET_FILES ${target_name} CXX_MODULE_SET_${MODULE_SET})
            if (MODULE_SET_FILES AND NOT MODULE_SET_FILES STREQUAL "MODULE_SET_FILES-NOTFOUND")
                list(APPEND ALL_SOURCES ${MODULE_SET_FILES})
            endif ()
        endforeach ()
    endif ()

    if (NOT ALL_SOURCES)
        set(${out_var} "" PARENT_SCOPE)
        return()
    endif ()

    list(REMOVE_DUPLICATES ALL_SOURCES)

    # Convert to absolute paths and filter by extension
    get_target_property(TARGET_SOURCE_DIR ${target_name} SOURCE_DIR)
    set(ABSOLUTE_SOURCES "")

    foreach (SOURCE_FILE ${ALL_SOURCES})
        # Make path absolute - use CMAKE_CURRENT_SOURCE_DIR for reliable resolution
        if (NOT IS_ABSOLUTE "${SOURCE_FILE}")
            get_filename_component(SOURCE_FILE "${SOURCE_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif ()

        # Filter to only include C++ source/header files
        if (SOURCE_FILE MATCHES "\\.(cpp|cppm|hpp|h|cc|cxx|ixx)$")
            # Only include files within the project source directory
            if (SOURCE_FILE MATCHES "^${CMAKE_SOURCE_DIR}/")
                list(APPEND ABSOLUTE_SOURCES ${SOURCE_FILE})
            endif ()
        endif ()
    endforeach ()

    set(${out_var} "${ABSOLUTE_SOURCES}" PARENT_SCOPE)
endfunction()

# ============================================================================
# Registration functions
# ============================================================================

# Single function to register a target for all code quality tools
function(register_code_quality target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist")
        return()
    endif ()

    # Register for formatting
    if (COMMAND add_format_target)
        add_format_target(${target_name})
    endif ()

    # Register for tidy (if enabled)
    if (COMMAND enable_clang_tidy_for_target)
        enable_clang_tidy_for_target(${target_name})
    endif ()

    if (COMMAND add_tidy_target)
        add_tidy_target(${target_name})
    endif ()
endfunction()


# Individual tool wrappers (for fine-grained control)
function(register_target_for_formatting target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist")
        return()
    endif ()

    if (COMMAND add_format_target)
        add_format_target(${target_name})
    endif ()
endfunction()

function(register_target_for_tidy target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist")
        return()
    endif ()

    if (COMMAND enable_clang_tidy_for_target)
        enable_clang_tidy_for_target(${target_name})
    endif ()

    if (COMMAND add_tidy_target)
        add_tidy_target(${target_name})
    endif ()
endfunction()

# Summary
message(STATUS "CodeQuality helper module loaded")
message(STATUS "Usage:")
message(STATUS "  - register_code_quality(target_name)         - Register for all tools")
message(STATUS "  - register_target_for_formatting(target_name) - Register for format only")
message(STATUS "  - register_target_for_tidy(target_name)       - Register for tidy only")