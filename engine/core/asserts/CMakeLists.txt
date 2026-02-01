add_library(RhodoCoreAsserts STATIC)


message("Configuring RhodoCoreAsserts module source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
# Module interface units
target_sources(RhodoCoreAsserts
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Asserts.cppm
)

# Include directory
target_include_directories(RhodoCoreAsserts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RhodoCoreAsserts PUBLIC
        RhodoCoreLogger
)

set_target_output_dirs(RhodoCoreAsserts)
register_code_quality(RhodoCoreAsserts)