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

# Collect all sources
if (COMMAND rh_collect_sources)
    rh_collect_sources(ALL_SOURCES)
endif()

if (ALL_SOURCES)
    # Create main format target
    add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCES}
            COMMENT "Formatting sources..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
            COMMAND_EXPAND_LISTS
    )

    # Create format-check target
    add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror -style=file ${ALL_SOURCES}
            COMMENT "Checking formatting..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
            COMMAND_EXPAND_LISTS
    )
endif()

