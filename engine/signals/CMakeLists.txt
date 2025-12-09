add_library(RhodoSignals STATIC)


message("Configuring RhodoSignals module source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
# Module interface units
target_sources(RhodoSignals
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Signal.cppm
        ScopedConnection.cppm
        SignalHub.cppm
        Signals.cppm
)

# Include directory
target_include_directories(RhodoSignals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RhodoSignals PUBLIC RhodoDebug)

# Register engine-specific clang-tidy config (performance-first LLVM style)
register_tidy_config(RhodoSignals ${CMAKE_SOURCE_DIR}/engine)

register_code_quality(RhodoSignals)