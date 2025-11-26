add_subdirectory(Core)
add_subdirectory(Logger)
add_subdirectory(Signals)

# Create static library
add_library(TestsRhodo STATIC)

target_sources(TestsRhodo
        PRIVATE

)


# Link libraries
target_link_libraries(TestsRhodo
        PUBLIC
        TestsRhodoCore
        TestsRhodoLogger
        TestsRhodoSignals
)

