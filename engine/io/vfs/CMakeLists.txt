add_library(RhodoIoVfs STATIC)


# Module interface units
target_sources(RhodoIoVfs
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Vfs.cppm
)

# Include directory
target_include_directories(RhodoIoVfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set_target_output_dirs(RhodoIoVfs)
register_code_quality(RhodoIoVfs)