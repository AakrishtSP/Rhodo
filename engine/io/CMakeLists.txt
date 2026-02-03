add_subdirectory(packing)
add_subdirectory(vfs)

# Create static library
add_library(RhodoIo STATIC)

target_sources(RhodoIo
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Io.cppm
)

# Link libraries
target_link_libraries(RhodoIo
        PUBLIC
        RhodoIoVfs
        RhodoIoPacking
)

# Set output directories
set_target_output_dirs(RhodoIo)
register_code_quality(RhodoIo)
