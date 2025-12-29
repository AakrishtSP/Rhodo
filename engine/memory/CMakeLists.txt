add_library(RhodoMemory STATIC)

target_sources(RhodoMemory
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Memory.cppm
        Category.cppm
        Hooks.cppm
        Allocator.cppm
)

target_link_libraries(RhodoMemory
        PRIVATE
        RhodoLogger
)

target_include_directories(RhodoMemory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

register_code_quality(RhodoMemory)
