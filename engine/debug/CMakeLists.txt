add_library(RhodoDebug STATIC)

target_sources(RhodoDebug
        PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
        Debug.cppm
)

target_link_libraries(RhodoDebug
        PRIVATE
        RhodoLogger
)

target_include_directories(RhodoDebug PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Register engine-specific clang-tidy config (performance-first LLVM style)
register_tidy_config(RhodoDebug ${CMAKE_SOURCE_DIR}/engine)

register_code_quality(RhodoDebug)
