add_library(TestsApplication OBJECT ApplicationTests.cpp)

target_link_libraries(TestsApplication
    PUBLIC
        Catch2::Catch2
)
