add_library(TestsApplication STATIC)

target_sources(TestsApplication
    PUBLIC

    PRIVATE

)

target_include_directories(TestsApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
