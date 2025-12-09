# EnforceLayout.cmake - Rhodo Engine Naming & Layout Enforcement
# Fails CMake configuration if naming conventions are violated

# ============================================================================
# Core Enforcement Function
# ============================================================================

function(enforce_naming dir regex description)
    if (NOT EXISTS "${dir}")
        message(DEBUG "Skipping enforcement for non-existent directory: ${dir}")
        return()
    endif ()

    file(GLOB_RECURSE ALL_PATHS
            LIST_DIRECTORIES true
            RELATIVE "${dir}"
            "${dir}/*")

    set(VIOLATIONS "")

    foreach (rel_path IN LISTS ALL_PATHS)
        get_filename_component(name "${rel_path}" NAME)

        # Skip common ignored patterns
        if (name MATCHES "^\\..*")  # Hidden files/dirs
            continue()
        endif ()
        if (name MATCHES "^(CMakeFiles|cmake_install\\.cmake|Makefile|.*\\.tmp)$")
            continue()
        endif ()

        # Check against regex
        if (NOT name MATCHES "${regex}")
            list(APPEND VIOLATIONS "${rel_path}")
        endif ()
    endforeach ()

    if (VIOLATIONS)
        list(LENGTH VIOLATIONS VIOLATION_COUNT)
        message(FATAL_ERROR
                "\n"
                "╔═══════════════════════════════════════════════════════════════════╗\n"
                "║  ❌ NAMING VIOLATION: ${description}\n"
                "╚═══════════════════════════════════════════════════════════════════╝\n"
                "\n"
                "Found ${VIOLATION_COUNT} violation(s) in: ${dir}\n"
                "\n"
                "Expected pattern: ${regex}\n"
                "\n"
                "Violations:\n"
                "  ${VIOLATIONS}\n"
                "\n"
                "Fix these files/directories to match the naming convention.\n"
        )
    endif ()
endfunction()

# ============================================================================
# Selective File Enforcement (ignore directories)
# ============================================================================

function(enforce_file_naming dir regex description extensions)
    if (NOT EXISTS "${dir}")
        message(DEBUG "Skipping enforcement for non-existent directory: ${dir}")
        return()
    endif ()

    set(VIOLATIONS "")
    set(PATTERN_LIST "")

    foreach (ext IN LISTS extensions)
        list(APPEND PATTERN_LIST "${dir}/**${ext}")
    endforeach ()

    foreach (pattern IN LISTS PATTERN_LIST)
        file(GLOB_RECURSE FILES
                RELATIVE "${dir}"
                "${pattern}")

        foreach (rel_path IN LISTS FILES)
            get_filename_component(name "${rel_path}" NAME)

            if (NOT name MATCHES "${regex}")
                list(APPEND VIOLATIONS "${rel_path}")
            endif ()
        endforeach ()
    endforeach ()

    if (VIOLATIONS)
        list(LENGTH VIOLATIONS VIOLATION_COUNT)
        message(FATAL_ERROR
                "\n"
                "╔═══════════════════════════════════════════════════════════════════╗\n"
                "║  ❌ FILE NAMING VIOLATION: ${description}\n"
                "╚═══════════════════════════════════════════════════════════════════╝\n"
                "\n"
                "Found ${VIOLATION_COUNT} violation(s) in: ${dir}\n"
                "\n"
                "Expected pattern: ${regex}\n"
                "Extensions checked: ${extensions}\n"
                "\n"
                "Violations:\n"
                "  ${VIOLATIONS}\n"
                "\n"
                "Fix these files to match the naming convention.\n"
        )
    endif ()
endfunction()

# ============================================================================
# Module File = Module Name Enforcement
# ============================================================================

function(enforce_module_names dir)
    if (NOT EXISTS "${dir}")
        return()
    endif ()

    file(GLOB_RECURSE MODULE_FILES
            "${dir}/*.cppm"
            "${dir}/*.ixx")

    set(VIOLATIONS "")

    foreach (module_path IN LISTS MODULE_FILES)
        get_filename_component(file_stem "${module_path}" NAME_WE)
        file(RELATIVE_PATH rel_path "${dir}" "${module_path}")
        get_filename_component(subdir "${rel_path}" DIRECTORY)

        # Read first 50 lines to find module declaration
        file(STRINGS "${module_path}" MODULE_LINES LIMIT_COUNT 50)

        set(DECLARED_NAME "")
        set(IS_PARTITION OFF)
        foreach (line IN LISTS MODULE_LINES)
            # Match: export module ModuleName; or module ModuleName:Partition;
            if (line MATCHES "^[ \t]*(export[ \t]+)?module[ \t]+([A-Za-z][A-Za-z0-9.]*)(:[A-Za-z][A-Za-z0-9]*)?")
                set(DECLARED_NAME "${CMAKE_MATCH_2}")
                if (CMAKE_MATCH_3)
                    set(PARTITION_NAME "${CMAKE_MATCH_3}")
                    string(SUBSTRING "${PARTITION_NAME}" 1 -1 PARTITION_NAME)  # Remove ':'
                    set(IS_PARTITION ON)
                endif ()
                break()
            endif ()
        endforeach ()

        if (NOT DECLARED_NAME)
            continue()
        endif ()

        # Validate module name is PascalCase (Rhodo.Logger, Rhodo.Signals)
        if (NOT DECLARED_NAME MATCHES "^Rhodo(\\.[A-Z][A-Za-z0-9]*)*$")
            list(APPEND VIOLATIONS
                    "\n  ❌ Invalid module name: ${DECLARED_NAME}"
                    "\n     File: ${rel_path}"
                    "\n     Must be: Rhodo.PascalCase (e.g., Rhodo.Logger, Rhodo.Signals)")
        endif ()

        # Check filename matches module name rules
        if (IS_PARTITION)
            # Partition: Rhodo.Logger:Impl -> Impl.cppm (must match partition name in PascalCase)
            # The filename must be in PascalCase and can be either:
            # - Just the partition name: Impl.cppm for Rhodo.Logger:Impl
            # - Prefixed with last module component: LoggerImpl.cppm for Rhodo.Logger:Impl
            # - Or other PascalCase variants: ScopedConnection.cppm, SignalHub.cppm

            # Validate filename is PascalCase
            if (NOT file_stem MATCHES "^[A-Z][A-Za-z0-9]*$")
                list(APPEND VIOLATIONS
                        "\n  ❌ Partition filename not PascalCase: ${file_stem}.cppm"
                        "\n     Module: ${DECLARED_NAME}:${PARTITION_NAME}"
                        "\n     File: ${rel_path}"
                        "\n     Filenames must be PascalCase (e.g., Impl, ScopedConnection, SignalHub)")
            endif ()
        else ()
            # Primary module: Rhodo.Logger -> Logger.cppm
            string(REGEX REPLACE "^.*\\.([^.]+)$" "\\1" last_component "${DECLARED_NAME}")

            # root module: Rhodo -> Rhodo.cppm
            if (DECLARED_NAME STREQUAL "Rhodo")
                set(last_component "Rhodo")
            endif ()

            if (NOT file_stem STREQUAL last_component)
                list(APPEND VIOLATIONS
                        "\n  ❌ Module filename mismatch: ${file_stem}.cppm"
                        "\n     Module: ${DECLARED_NAME}"
                        "\n     Expected: ${last_component}.cppm"
                        "\n     File: ${rel_path}")
            endif ()
        endif ()
    endforeach ()

    if (VIOLATIONS)
        message(FATAL_ERROR
                "\n"
                "╔═══════════════════════════════════════════════════════════════════╗\n"
                "║  ❌ MODULE NAMING VIOLATIONS (PascalCase Scheme)\n"
                "╚═══════════════════════════════════════════════════════════════════╝\n"
                "\n"
                "Rhodo enforces strict PascalCase hierarchical module naming:\n"
                "\n"
                "Rules:\n"
                "  • root:      export module Rhodo;           → Rhodo.cppm\n"
                "  • Subsystem: export module Rhodo.Logger;    → Logger.cppm\n"
                "  • Partition: export module Rhodo.Logger:Impl; → Impl.cppm\n"
                "  • Max depth: 3 levels (Rhodo.Module.SubModule)\n"
                "\n"
                "Violations:${VIOLATIONS}\n"
                "\n"
                "Fix by renaming modules to match PascalCase scheme.\n"
        )
    endif ()
endfunction()

# ============================================================================
# Rhodo Engine Naming Rules
# ============================================================================

option(RH_ENFORCE_NAMING "Enforce strict naming conventions" ON)
option(RH_ENFORCE_MODULE_NAME_MATCH "Enforce PascalCase hierarchical module naming" ON)

if (RH_ENFORCE_NAMING)
    message(STATUS "Enforcing Rhodo naming conventions...")

    # ✅ Engine Core: CamelCase files, no underscores
    # Pattern: Logger.cpp, EntityManager.hpp, Debug.cppm
    enforce_file_naming(
            "${CMAKE_SOURCE_DIR}/engine"
            "^[A-Z][A-Za-z0-9]+(\\.(cpp|cppm|ixx|hpp|h))?$"
            "Engine source files (must be CamelCase)"
            ".cpp;.cppm;.ixx;.hpp;.h"
    )

    # ✅ Application/Tools: snake_case or CamelCase allowed
    # Pattern: main.cpp, Application.cpp, editor_window.cpp
    enforce_file_naming(
            "${CMAKE_SOURCE_DIR}/application"
            "^[A-Za-z][A-Za-z0-9_]+(\\.(cpp|hpp|h))?$"
            "Application source files (CamelCase or snake_case)"
            ".cpp;.hpp;.h"
    )

    # ✅ Tests: flexible naming for test frameworks
    # Pattern: test_logger.cpp, LoggerTests.cpp
    enforce_file_naming(
            "${CMAKE_SOURCE_DIR}/tests"
            "^[A-Za-z][A-Za-z0-9_]+(\\.(cpp|hpp|h))?$"
            "Test files (flexible naming)"
            ".cpp;.hpp;.h"
    )

    # ✅ CMake files: snake_case with .cmake extension
    # Pattern: clang_tidy.cmake, enforce_layout.cmake
    enforce_file_naming(
            "${CMAKE_SOURCE_DIR}/cmake"
            "^[A-Z][A-Za-z0-9]+(\\.(cmake|txt))?$"
            "CMake module files (CamelCase)"
            ".cmake"
    )

    # ✅ Module name = filename enforcement (optional, can be strict)
    # Note: Disabled by default as module hierarchies (rhodo.logger in Logger.cppm) are valid
    # To enable: -DRH_ENFORCE_MODULE_NAME_MATCH=ON
    if (RH_ENFORCE_MODULE_NAME_MATCH)
        enforce_module_names("${CMAKE_SOURCE_DIR}/Rhodo")
    endif ()

    message(STATUS "✅ All naming conventions validated")
else ()
    message(STATUS "⚠️  Naming enforcement disabled (RH_ENFORCE_NAMING=OFF)")
endif ()
