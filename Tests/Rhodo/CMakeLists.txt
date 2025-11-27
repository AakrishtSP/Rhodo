add_subdirectory(Core)
add_subdirectory(Logger)
add_subdirectory(Signals)


add_library(TestsRhodo STATIC)

target_link_libraries(TestsRhodo
        PUBLIC
        TestsRhodoCore
        TestsRhodoLogger
        TestsRhodoSignals
)