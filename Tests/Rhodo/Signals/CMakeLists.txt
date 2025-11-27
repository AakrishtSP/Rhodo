add_library(TestsRhodoSignals OBJECT SignalTests.cpp)

target_link_libraries(TestsRhodoSignals
    PUBLIC
        Rhodo
        Catch2::Catch2
)