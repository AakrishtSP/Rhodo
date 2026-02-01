add_library(RhodoInfrastructureInstrumentation STATIC)

add_subdirectory(memory)
add_subdirectory(signals)
add_subdirectory(profiling)

target_sources(RhodoInfrastructureInstrumentation
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Instrumentation.cppm
)

target_link_libraries(RhodoInfrastructureInstrumentation
    PUBLIC
    RhodoInfrastructureInstrumentationMemory
    RhodoInfrastructureInstrumentationSignals
    RhodoInfrastructureInstrumentationProfiling
)
