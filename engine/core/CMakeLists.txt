add_subdirectory(asserts)
add_subdirectory(logger)
add_subdirectory(memory)
add_subdirectory(signals)

# Create static library
add_library(RhodoCore STATIC)

target_sources(RhodoCore
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Core.cppm
)

# Link libraries
target_link_libraries(RhodoCore
        PUBLIC
        RhodoCoreAsserts
        RhodoCoreLogger
        RhodoCoreMemory
        RhodoCoreSignals
)

# Set output directories
set_target_output_dirs(RhodoCore)
register_code_quality(RhodoCore)
