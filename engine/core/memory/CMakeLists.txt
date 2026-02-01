add_library(RhodoCoreMemory STATIC)

target_sources(RhodoCoreMemory
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Memory.cppm
        Category.cppm
        Hooks.cppm
        Allocator.cppm
)

target_link_libraries(RhodoCoreMemory
        PRIVATE
        RhodoCoreLogger
)

target_include_directories(RhodoCoreMemory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_output_dirs(RhodoCoreMemory)
register_code_quality(RhodoCoreMemory)
