add_library(RhodoInstrumentationSignals STATIC)

target_sources(RhodoInstrumentationSignals
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    SignalProfiler.cppm
)

target_link_libraries(RhodoInstrumentationSignals
    PRIVATE
    RhodoSignals # Depends on the Core Signals module
    RhodoLogger
)
