add_library(RhodoIoPacking STATIC)


# Module interface units
target_sources(RhodoIoPacking
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Packing.cppm
)

# Include directory
target_include_directories(RhodoIoPacking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set_target_output_dirs(RhodoIoPacking)
register_code_quality(RhodoIoPacking)