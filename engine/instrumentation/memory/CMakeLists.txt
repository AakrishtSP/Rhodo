add_library(RhodoInstrumentationMemory STATIC)

target_sources(RhodoInstrumentationMemory
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    MemoryProfiler.cppm
)

target_link_libraries(RhodoInstrumentationMemory
    PRIVATE
    RhodoMemory # Depends on the Core Memory module
    RhodoLogger
)
