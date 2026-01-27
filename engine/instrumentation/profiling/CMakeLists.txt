add_library(RhodoInstrumentationProfiling STATIC)

target_sources(RhodoInstrumentationProfiling
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Profiler.cppm
)

target_link_libraries(RhodoInstrumentationProfiling
    PRIVATE
    RhodoDebug
    RhodoLogger
)
