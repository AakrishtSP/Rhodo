add_library(TestsRhodoSignals STATIC SignalTests.cpp)

target_link_libraries(TestsRhodoSignals
    PUBLIC
        Catch2::Catch2
        Rhodo
)