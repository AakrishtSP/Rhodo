add_library(RhodoInfrastructureInstrumentationMemory STATIC)

target_sources(RhodoInfrastructureInstrumentationMemory
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    MemoryProfiler.cppm
)

target_link_libraries(RhodoInfrastructureInstrumentationMemory
    PRIVATE
    RhodoCoreMemory # Depends on the Core Memory module
    RhodoCoreLogger
)
