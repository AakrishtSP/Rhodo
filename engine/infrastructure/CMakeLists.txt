add_subdirectory(debug)
add_subdirectory(instrumentation)

# Create static library
add_library(RhodoInfrastructure STATIC)

target_sources(RhodoInfrastructure
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Infrastructure.cppm
)

# Link libraries
target_link_libraries(RhodoInfrastructure
        PUBLIC
        RhodoInfrastructureInstrumentation
        RhodoInfrastructureDebug
)

# Set output directories
set_target_output_dirs(RhodoInfrastructure)
register_code_quality(RhodoInfrastructure)
