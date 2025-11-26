add_library(TestsRhodoLogger STATIC)

target_sources(TestsRhodoLogger
    PUBLIC

    PRIVATE

)

target_include_directories(TestsRhodoLogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
