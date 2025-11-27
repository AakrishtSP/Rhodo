add_library(TestsRhodoCore STATIC)

target_sources(TestsRhodoCore
    PRIVATE
    CoreTests.cpp
)

target_include_directories(TestsRhodoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
