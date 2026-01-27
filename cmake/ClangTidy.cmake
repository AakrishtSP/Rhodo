# ClangTidy.cmake - Clang-Tidy Integration Module

if (NOT RH_ENABLE_CLANG_TIDY)
    return()
endif ()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if (NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy requested but not found!")
    set(RH_ENABLE_CLANG_TIDY OFF CACHE BOOL "Enable clang-tidy checks" FORCE)
    return()
endif ()

message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")

# Function to enable clang-tidy for a target (runs during build)
function(rh_enable_clang_tidy target_name)
    if (TARGET ${target_name})
        set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
        )
    endif()
endfunction()

# Optional: Global tidy target for manual checks
if (COMMAND rh_collect_sources)
    rh_collect_sources(ALL_SOURCES)
endif()

# Filter to C++ implementation files
set(CPP_SOURCES "")
foreach (SOURCE_FILE ${ALL_SOURCES})
    if (SOURCE_FILE MATCHES "\\.(cpp|cppm|ixx|cc|cxx)$")
        list(APPEND CPP_SOURCES ${SOURCE_FILE})
    endif ()
endforeach ()

if (CPP_SOURCES)
    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${CPP_SOURCES}
        COMMENT "Running clang-tidy..."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )

    add_custom_target(tidy-fix
        COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} --fix ${CPP_SOURCES}
        COMMENT "Running clang-tidy fix..."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
endif()

