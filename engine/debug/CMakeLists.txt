add_library(RhodoDebug STATIC)

target_sources(RhodoDebug
    PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Debug.cppm
    Profiling.cppm
)
