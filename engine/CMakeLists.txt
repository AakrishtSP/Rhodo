if (RH_ENABLE_INSTRUMENTATION)
    message("Enabled Instrumentation")
    add_subdirectory(infrastructure)
    add_compile_definitions(RH_ENABLE_INSTRUMENTATION)
endif ()

add_subdirectory(core)
add_subdirectory(io)

# Create static library
add_library(Rhodo STATIC)

target_sources(Rhodo
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Rhodo.cppm
)

# Link libraries
target_link_libraries(Rhodo
        PUBLIC
        RhodoCore
        RhodoIo
)

# Conditionally link instrumentation
if (RH_ENABLE_INSTRUMENTATION)
    target_link_libraries(Rhodo PUBLIC RhodoInfrastructureInstrumentation)
endif ()

# Build configuration defines (works with both single-config and multi-config generators)
target_compile_definitions(Rhodo
        PRIVATE
        RH_BUILD_STATIC

        # Debug configuration
        $<$<CONFIG:Debug>:RH_DEBUG;RHODO_LOGGING;RH_ENABLE_ASSERTS;RH_ENABLE_EVENT_NAMES>
        # Release with debug info configuration
        $<$<CONFIG:RelWithDebInfo>:RH_RELEASE;RH_ENABLE_LOGGING;RH_ENABLE_EVENT_NAMES>
        # Release configuration
        $<$<CONFIG:Release>:RH_RELEASE>
        # MinSizeRel configuration
        $<$<CONFIG:MinSizeRel>:RH_RELEASE>
)

# Set output directories
set_target_output_dirs(Rhodo)

register_code_quality(Rhodo)

