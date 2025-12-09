# ClangTidy.cmake - Clang-Tidy Integration Module


# Global variables to collect sources (always initialize)
set_property(GLOBAL PROPERTY RH_ALL_TIDY_SOURCES "")
set_property(GLOBAL PROPERTY RH_TIDY_TARGETS "")

# Always define functions, but only execute tidy logic if enabled
set(RH_CLANG_TIDY_AVAILABLE OFF)

if (RH_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    find_program(RUN_CLANG_TIDY_EXE NAMES run-clang-tidy)

    if (NOT CLANG_TIDY_EXE)
        message(WARNING "clang-tidy requested but not found!")
        set(RH_ENABLE_CLANG_TIDY OFF CACHE BOOL "Enable clang-tidy checks" FORCE)
    else ()
        set(RH_CLANG_TIDY_AVAILABLE ON)

        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")

        # Check clang-tidy version
        execute_process(
                COMMAND ${CLANG_TIDY_EXE} --version
                OUTPUT_VARIABLE CLANG_TIDY_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        message(STATUS "clang-tidy version: ${CLANG_TIDY_VERSION}")

        # Extract version number
        string(REGEX MATCH "version ([0-9]+)\\.([0-9]+)" _ ${CLANG_TIDY_VERSION})
        set(CLANG_TIDY_VERSION_MAJOR ${CMAKE_MATCH_1})
        set(CLANG_TIDY_VERSION_MINOR ${CMAKE_MATCH_2})

        if (CLANG_TIDY_VERSION_MAJOR LESS 14)
            message(WARNING "clang-tidy version ${CLANG_TIDY_VERSION_MAJOR}.${CLANG_TIDY_VERSION_MINOR} detected. Version 14+ recommended.")
        endif ()

        # Limit analysis to project sources (skip build/_deps/thirdparty noise)
        set(CLANG_TIDY_HEADER_FILTER "${CMAKE_SOURCE_DIR}/(application|engine|tests)/.*")

        # Configure clang-tidy command (will be overridden per-target)
        set(CMAKE_CXX_CLANG_TIDY
                "${CLANG_TIDY_EXE}"
                "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
                "--header-filter=${CLANG_TIDY_HEADER_FILTER}"
        )

        # Map directories to their respective .clang-tidy configs
        set_property(GLOBAL PROPERTY RH_CLANG_TIDY_CONFIG_MAP "")

        message(STATUS "Note: clang-tidy support for C++20 modules is evolving, analysis of .cppm/.ixx files enabled")

        if (RUN_CLANG_TIDY_EXE)
            message(STATUS "run-clang-tidy found: ${RUN_CLANG_TIDY_EXE} (parallel execution enabled)")
        else ()
            message(STATUS "run-clang-tidy not found - falling back to direct clang-tidy (slower)")
        endif ()
    endif ()
endif ()


# Helper functions to enable/disable clang-tidy for specific targets

# Function to register a directory-specific clang-tidy config
function(register_tidy_config target_name config_dir)
    if (NOT RH_CLANG_TIDY_AVAILABLE)
        return()
    endif ()

    # Check if config file exists in the specified directory
    set(CONFIG_FILE "${config_dir}/.clang-tidy")
    if (NOT EXISTS "${CONFIG_FILE}")
        message(STATUS "No .clang-tidy found in ${config_dir}, will use root config")
        return()
    endif ()

    message(STATUS "Registering custom clang-tidy config for ${target_name}: ${CONFIG_FILE}")

    # Store the mapping globally
    get_property(CONFIG_MAP GLOBAL PROPERTY RH_CLANG_TIDY_CONFIG_MAP)
    list(APPEND CONFIG_MAP "${target_name}:${CONFIG_FILE}")
    list(REMOVE_DUPLICATES CONFIG_MAP)
    set_property(GLOBAL PROPERTY RH_CLANG_TIDY_CONFIG_MAP "${CONFIG_MAP}")
endfunction()

# Function to get the appropriate clang-tidy config for a target
function(get_tidy_config_for_target target_name out_var)
    # Check if target has a registered custom config
    get_property(CONFIG_MAP GLOBAL PROPERTY RH_CLANG_TIDY_CONFIG_MAP)

    foreach (MAPPING ${CONFIG_MAP})
        string(REPLACE ":" ";" PARTS ${MAPPING})
        list(GET PARTS 0 MAPPED_TARGET)
        list(GET PARTS 1 CONFIG_FILE)

        if (MAPPED_TARGET STREQUAL target_name)
            set(${out_var} "${CONFIG_FILE}" PARENT_SCOPE)
            return()
        endif ()
    endforeach ()

    # Default to root config
    set(${out_var} "${CMAKE_SOURCE_DIR}/.clang-tidy" PARENT_SCOPE)
endfunction()

function(enable_clang_tidy_for_target target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist")
        return()
    endif ()

    # Only proceed if clang-tidy is available
    if (NOT RH_CLANG_TIDY_AVAILABLE)
        return()
    endif ()

    # Get the appropriate config file for this target
    get_tidy_config_for_target(${target_name} TIDY_CONFIG)

    set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--config-file=${TIDY_CONFIG};--header-filter=${CLANG_TIDY_HEADER_FILTER}"
    )

    # Use shared source collection from CodeQuality.cmake
    if (COMMAND collect_target_sources)
        collect_target_sources(${target_name} ALL_SOURCES)
    else ()
        message(WARNING "collect_target_sources function not found! Include CodeQuality.cmake first.")
        return()
    endif ()

    if (ALL_SOURCES)
        # Filter to C++ implementation files including modules
        set(CPP_SOURCES "")
        foreach (SOURCE_FILE ${ALL_SOURCES})
            if (SOURCE_FILE MATCHES "\\.(cpp|cppm|ixx)$")
                list(APPEND CPP_SOURCES ${SOURCE_FILE})
            endif ()
        endforeach ()

        if (CPP_SOURCES)
            # Add to global list
            get_property(GLOBAL_SOURCES GLOBAL PROPERTY RH_ALL_TIDY_SOURCES)
            list(APPEND GLOBAL_SOURCES ${CPP_SOURCES})
            list(REMOVE_DUPLICATES GLOBAL_SOURCES)
            set_property(GLOBAL PROPERTY RH_ALL_TIDY_SOURCES "${GLOBAL_SOURCES}")

            # Register target
            get_property(ALL_TARGETS GLOBAL PROPERTY RH_TIDY_TARGETS)
            list(APPEND ALL_TARGETS ${target_name})
            set_property(GLOBAL PROPERTY RH_TIDY_TARGETS "${ALL_TARGETS}")

            message(STATUS "Enabled clang-tidy for target: ${target_name}")
        endif ()
    endif ()
endfunction()

function(disable_clang_tidy_for_target target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist")
        return()
    endif ()

    set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY ""
    )
    message(STATUS "Disabled clang-tidy for target: ${target_name}")
endfunction()


# Function to create manual tidy targets after all targets are registered
function(finalize_tidy_targets)
    get_property(ALL_SOURCES GLOBAL PROPERTY RH_ALL_TIDY_SOURCES)
    get_property(ALL_TARGETS GLOBAL PROPERTY RH_TIDY_TARGETS)

    if (NOT ALL_SOURCES)
        message(WARNING "No sources registered for clang-tidy!")
        return()
    endif ()

    list(LENGTH ALL_SOURCES NUM_FILES)
    list(LENGTH ALL_TARGETS NUM_TARGETS)

    # Run clang-tidy on all registered files (use run-clang-tidy if available for parallel execution)
    if (RUN_CLANG_TIDY_EXE)
        add_custom_target(tidy
                COMMAND ${CMAKE_COMMAND} -E echo "[clang-tidy] Analyzing ${NUM_FILES} files from ${NUM_TARGETS} targets (parallel mode)..."
                COMMAND ${RUN_CLANG_TIDY_EXE}
                -p ${CMAKE_BINARY_DIR}
                COMMENT "Running clang-tidy (parallel) - check output for progress"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM
                USES_TERMINAL
        )
    else ()
        add_custom_target(tidy
                COMMAND ${CLANG_TIDY_EXE}
                --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
                -p ${CMAKE_BINARY_DIR}
                ${ALL_SOURCES}
                COMMENT "Running clang-tidy on ${NUM_FILES} files from ${NUM_TARGETS} targets..."
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM
        )
    endif ()

    # Fix issues automatically (use run-clang-tidy if available for parallel execution)
    if (RUN_CLANG_TIDY_EXE)
        add_custom_target(tidy-fix
                COMMAND ${CMAKE_COMMAND} -E echo "[clang-tidy] Auto-fixing ${NUM_FILES} files from ${NUM_TARGETS} targets (parallel mode)..."
                COMMAND ${RUN_CLANG_TIDY_EXE}
                -p ${CMAKE_BINARY_DIR}
                -fix
                COMMENT "Running clang-tidy auto-fix (parallel) - check output for progress"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM
                USES_TERMINAL
        )
    else ()
        add_custom_target(tidy-fix
                COMMAND ${CLANG_TIDY_EXE}
                --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
                -p ${CMAKE_BINARY_DIR}
                --fix
                ${ALL_SOURCES}
                COMMENT "Running clang-tidy with auto-fix on ${NUM_FILES} files..."
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM
        )
    endif ()

    message(STATUS "Clang-tidy targets created for ${NUM_TARGETS} targets (${NUM_FILES} .cpp files)")
endfunction()


# Function to create per-target tidy targets
function(add_tidy_target target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "Target ${target_name} does not exist, skipping tidy registration")
        return()
    endif ()

    # Only proceed if clang-tidy is available
    if (NOT RH_CLANG_TIDY_AVAILABLE)
        return()
    endif ()

    # Use shared source collection from CodeQuality.cmake
    if (COMMAND collect_target_sources)
        collect_target_sources(${target_name} ALL_SOURCES)
    else ()
        message(WARNING "collect_target_sources function not found! Include CodeQuality.cmake first.")
        return()
    endif ()

    # Filter to C++ implementation files including modules
    set(CPP_SOURCES "")
    foreach (SOURCE_FILE ${ALL_SOURCES})
        if (SOURCE_FILE MATCHES "\\.(cpp|cppm|ixx)$")
            list(APPEND CPP_SOURCES ${SOURCE_FILE})
        endif ()
    endforeach ()

    if (NOT CPP_SOURCES)
        return()
    endif ()

    # Create per-target tidy target
    get_tidy_config_for_target(${target_name} TIDY_CONFIG)

    add_custom_target(tidy-${target_name}
            COMMAND ${CLANG_TIDY_EXE}
            --config-file=${TIDY_CONFIG}
            -p ${CMAKE_BINARY_DIR}
            ${CPP_SOURCES}
            COMMENT "Running clang-tidy on target: ${target_name} (config: ${TIDY_CONFIG})"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )

    list(LENGTH CPP_SOURCES NUM_FILES)
    message(STATUS "Created tidy-${target_name} for ${NUM_FILES} .cpp files")
endfunction()


# Function to add tidy target for a group of targets
function(add_tidy_group group_name)
    set(GROUP_TARGETS ${ARGN})

    if (NOT GROUP_TARGETS)
        message(WARNING "No targets provided for tidy group ${group_name}")
        return()
    endif ()

    # Collect all sources and their configs from the group using shared function
    set(GROUP_SOURCES "")
    set(GROUP_CONFIGS "")

    foreach (TARGET_NAME ${GROUP_TARGETS})
        if (NOT TARGET ${TARGET_NAME})
            continue()
        endif ()

        if (COMMAND collect_target_sources)
            collect_target_sources(${TARGET_NAME} ALL_SOURCES)
            if (ALL_SOURCES)
                # Filter to only .cpp files
                foreach (SOURCE_FILE ${ALL_SOURCES})
                    if (SOURCE_FILE MATCHES "\\.cpp$")
                        list(APPEND GROUP_SOURCES ${SOURCE_FILE})
                    endif ()
                endforeach ()
            endif ()
        endif ()

        # Collect configs for this target
        get_tidy_config_for_target(${TARGET_NAME} TARGET_CONFIG)
        list(APPEND GROUP_CONFIGS ${TARGET_CONFIG})
    endforeach ()

    if (NOT GROUP_SOURCES)
        return()
    endif ()

    list(REMOVE_DUPLICATES GROUP_SOURCES)
    list(REMOVE_DUPLICATES GROUP_CONFIGS)

    # If multiple configs, use root config for consistency
    list(LENGTH GROUP_CONFIGS CONFIG_COUNT)
    if (CONFIG_COUNT GREATER 1)
        message(STATUS "Group '${group_name}' has targets with different configs, using root config")
        set(GROUP_CONFIG "${CMAKE_SOURCE_DIR}/.clang-tidy")
    else ()
        list(GET GROUP_CONFIGS 0 GROUP_CONFIG)
    endif ()

    add_custom_target(tidy-${group_name}
            COMMAND ${CLANG_TIDY_EXE}
            --config-file=${GROUP_CONFIG}
            -p ${CMAKE_BINARY_DIR}
            ${GROUP_SOURCES}
            COMMENT "Running clang-tidy on group: ${group_name}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )

    list(LENGTH GROUP_SOURCES NUM_FILES)
    message(STATUS "Created tidy-${group_name} for ${NUM_FILES} .cpp files")
endfunction()


# Convenience wrapper function
function(register_target_for_tidy target_name)
    if (COMMAND enable_clang_tidy_for_target)
        enable_clang_tidy_for_target(${target_name})
    endif ()

    if (COMMAND add_tidy_target)
        add_tidy_target(${target_name})
    endif ()
endfunction()


# Summary
message(STATUS "Clang-Tidy module loaded (ENABLED)")
message(STATUS "Usage:")
message(STATUS "  - Call register_tidy_config(target_name config_dir) before register_code_quality() to use per-directory configs")
message(STATUS "  - Call register_code_quality(target_name) after each target to enable tidy")
message(STATUS "  - Or call enable_clang_tidy_for_target(target_name) + add_tidy_target(target_name) separately")
message(STATUS "  - Call finalize_tidy_targets() at the end of root CMakeLists.txt")
message(STATUS "  - Optional: add_tidy_group(group_name target1 target2 ...) for grouped targets")