add_library(TestsRhodoSignals STATIC)

target_sources(TestsRhodoSignals
    PUBLIC
        SignalTests.cpp
    PRIVATE

)

target_include_directories(TestsRhodoSignals PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
