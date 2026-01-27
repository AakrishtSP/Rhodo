add_library(RhodoInstrumentation STATIC)

add_subdirectory(memory)
add_subdirectory(signals)
add_subdirectory(profiling)

target_sources(RhodoInstrumentation
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Instrumentation.cppm
)

target_link_libraries(RhodoInstrumentation
    PUBLIC
    RhodoInstrumentationMemory
    RhodoInstrumentationSignals
    RhodoInstrumentationProfiling
)
