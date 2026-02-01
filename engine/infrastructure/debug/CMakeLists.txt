add_library(RhodoInfrastructureDebug STATIC)

target_sources(RhodoInfrastructureDebug
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Debug.cppm
    Profiling.cppm
)

# Set output directories
set_target_output_dirs(RhodoInfrastructureDebug)
register_code_quality(RhodoInfrastructureDebug)

