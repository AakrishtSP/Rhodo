add_library(RhodoDebug STATIC)

target_sources(RhodoDebug
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Asserts.cppm
        Config.cppm
        Debug.cppm
        Memory.cppm
        Profiling.cppm
        ScopeGuards.cppm
)

target_link_libraries(RhodoDebug
        PRIVATE
        RhodoLogger
)

target_include_directories(RhodoDebug PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

register_code_quality(RhodoDebug)
