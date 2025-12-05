add_library(RhodoCore STATIC)

target_sources(RhodoCore
    PUBLIC
        Core.hpp
        PlatformDetection.h
    PRIVATE
        EntryPoint.cpp
)

target_link_libraries(RhodoCore
        PRIVATE
        RhodoLogger
)

target_include_directories(RhodoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

