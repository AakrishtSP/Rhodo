add_library(RhodoCore STATIC)

target_sources(RhodoCore
        PUBLIC
        Core.hpp
        PRIVATE
        EntryPoint.cpp
)

target_link_libraries(RhodoCore
        PRIVATE
        RhodoLogger
)

target_include_directories(RhodoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Register engine-specific clang-tidy config (performance-first LLVM style)
register_tidy_config(RhodoCore ${CMAKE_SOURCE_DIR}/engine)

register_code_quality(RhodoCore)

