add_subdirectory(Core)
add_subdirectory(Logger)
add_subdirectory(Signals)

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
        RhodoLogger
        RhodoSignals
)

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