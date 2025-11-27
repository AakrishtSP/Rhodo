add_library(TestsApplication STATIC)

target_sources(TestsApplication
    PRIVATE
    ApplicationTests.cpp
)

target_include_directories(TestsApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
