add_library(TestsRhodoLogger STATIC)

target_sources(TestsRhodoLogger
    PRIVATE
        LoggerTests.cpp
)

target_include_directories(TestsRhodoLogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
