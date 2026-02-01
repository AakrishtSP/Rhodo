add_library(RhodoInfrastructureInstrumentationProfiling STATIC)

target_sources(RhodoInfrastructureInstrumentationProfiling
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Profiler.cppm
)

target_link_libraries(RhodoInfrastructureInstrumentationProfiling
    PRIVATE
    RhodoInfrastructureDebug
    RhodoCoreLogger
)
