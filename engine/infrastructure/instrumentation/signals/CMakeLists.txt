add_library(RhodoInfrastructureInstrumentationSignals STATIC)

target_sources(RhodoInfrastructureInstrumentationSignals
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    SignalProfiler.cppm
)

target_link_libraries(RhodoInfrastructureInstrumentationSignals
    PRIVATE
    RhodoCoreSignals # Depends on the Core Signals module
    RhodoCoreLogger
)
