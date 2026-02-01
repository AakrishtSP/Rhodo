add_library(RhodoCoreSignals STATIC)


# Module interface units
target_sources(RhodoCoreSignals
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Signal.cppm
        ScopedConnection.cppm
        SignalHub.cppm
        Signals.cppm
        Hooks.cppm
        PRIVATE
        FILE_SET cxx_module_private TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        SignalImpl.cppm
        ScopedConnectionImpl.cppm
        SignalHubImpl.cppm
)

# Include directory
target_include_directories(RhodoCoreSignals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RhodoCoreSignals PUBLIC
        RhodoCoreLogger
)
set_target_output_dirs(RhodoCoreSignals)
register_code_quality(RhodoCoreSignals)