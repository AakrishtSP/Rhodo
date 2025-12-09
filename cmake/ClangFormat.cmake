# ClangFormat.cmake - Clang-Format Integration Module

if (NOT RH_ENABLE_CLANG_FORMAT)
    return()
endif ()

find_program(CLANG_FORMAT_EXE NAMES clang-format)

if (NOT CLANG_FORMAT_EXE)
    message(WARNING "clang-format not found! Format targets will not be available.")
    set(RH_ENABLE_CLANG_FORMAT OFF CACHE BOOL "Enable clang-format target" FORCE)
    return()
endif ()

message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")

# Get clang-format version
execute_process(
        COMMAND ${CLANG_FORMAT_EXE} --version
        OUTPUT_VARIABLE CLANG_FORMAT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "clang-format version: ${CLANG_FORMAT_VERSION}")


# Global variables to collect sources

# Initialize global lists for collecting sources
set_property(GLOBAL PROPERTY RH_ALL_FORMAT_SOURCES "")
set_property(GLOBAL PROPERTY RH_FORMAT_TARGETS "")


# Function to register a target for formatting
function(add_format_target target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist, skipping format registration")
        return()
    endif ()

    # Use shared source collection from CodeQuality.cmake
    if (COMMAND collect_target_sources)
        collect_target_sources(${target_name} ABSOLUTE_SOURCES)
    else ()
        message(WARNING "collect_target_sources function not found! Include CodeQuality.cmake first.")
        return()
    endif ()

    if (NOT ABSOLUTE_SOURCES)
        message(DEBUG "No sources found for target ${target_name}")
        return()
    endif ()

    get_property(ALL_SOURCES GLOBAL PROPERTY RH_ALL_FORMAT_SOURCES)
    list(APPEND ALL_SOURCES ${ABSOLUTE_SOURCES})
    list(REMOVE_DUPLICATES ALL_SOURCES)
    set_property(GLOBAL PROPERTY RH_ALL_FORMAT_SOURCES "${ALL_SOURCES}")

    get_property(ALL_TARGETS GLOBAL PROPERTY RH_FORMAT_TARGETS)
    list(APPEND ALL_TARGETS ${target_name})
    list(REMOVE_DUPLICATES ALL_TARGETS)
    set_property(GLOBAL PROPERTY RH_FORMAT_TARGETS "${ALL_TARGETS}")

    add_custom_target(format-${target_name}
            COMMAND ${CMAKE_COMMAND} -E echo "Formatting ${target_name}"
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ABSOLUTE_SOURCES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )
endfunction()

function(finalize_format_targets)
    get_property(ALL_SOURCES GLOBAL PROPERTY RH_ALL_FORMAT_SOURCES)
    get_property(ALL_TARGETS GLOBAL PROPERTY RH_FORMAT_TARGETS)

    if (NOT ALL_SOURCES)
        message(WARNING "No sources registered for formatting!")
        return()
    endif ()

    list(LENGTH ALL_SOURCES NUM_FILES)
    list(LENGTH ALL_TARGETS NUM_TARGETS)

    # Create main format target with verbose progress
    add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file -verbose ${ALL_SOURCES}
            COMMENT "Formatting ${NUM_FILES} files from ${NUM_TARGETS} targets..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
            COMMAND_EXPAND_LISTS
    )

    # Create format-check target with verbose progress
    add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror -style=file -verbose ${ALL_SOURCES}
            COMMENT "Checking formatting for ${NUM_FILES} files..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
            COMMAND_EXPAND_LISTS
    )

    message(STATUS "Format targets created for ${NUM_TARGETS} targets (${NUM_FILES} total files)")
endfunction()


# Convenience function to add format target for a group of targets
function(add_format_group group_name)
    set(GROUP_TARGETS ${ARGN})

    if (NOT GROUP_TARGETS)
        message(WARNING "No targets provided for format group ${group_name}")
        return()
    endif ()

    # Collect all sources from the group using shared function
    set(GROUP_SOURCES "")
    foreach (TARGET_NAME ${GROUP_TARGETS})
        if (NOT TARGET ${TARGET_NAME})
            continue()
        endif ()

        if (COMMAND collect_target_sources)
            collect_target_sources(${TARGET_NAME} TARGET_SOURCES)
            if (TARGET_SOURCES)
                list(APPEND GROUP_SOURCES ${TARGET_SOURCES})
            endif ()
        endif ()
    endforeach ()

    if (NOT GROUP_SOURCES)
        return()
    endif ()

    list(REMOVE_DUPLICATES GROUP_SOURCES)

    # Create group format target
    add_custom_target(format-${group_name}
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${GROUP_SOURCES}
            COMMENT "Formatting group: ${group_name}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )

    list(LENGTH GROUP_SOURCES NUM_FILES)
    message(STATUS "Created format-${group_name} for ${NUM_FILES} files")
endfunction()

# ============================================================================
# Convenience wrapper function
# ============================================================================

function(register_target_for_formatting target_name)
    if (COMMAND add_format_target)
        add_format_target(${target_name})
    endif ()
endfunction()


# Summary
message(STATUS "Clang-Format module loaded")
message(STATUS "Usage:")
message(STATUS "  - Call register_target_for_formatting(target_name) after each target")
message(STATUS "  - Or call add_format_target(target_name) directly")
message(STATUS "  - Call finalize_format_targets() at the end of root CMakeLists.txt")
message(STATUS "  - Optional: add_format_group(group_name target1 target2 ...) for grouped targets")