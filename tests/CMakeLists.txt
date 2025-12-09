# tests/CMakeLists.txt
project(RhodoTests LANGUAGES C CXX)

include(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/Catch2.cmake)

add_subdirectory(application)
add_subdirectory(engine)

add_executable(RhodoTests)

target_link_libraries(RhodoTests
        PRIVATE
        TestsApplication
        TestsRhodoSignals
        Rhodo
        Catch2::Catch2WithMain
)

set_target_output_dirs(RhodoTests)

include(CTest)
include(Catch)
catch_discover_tests(RhodoTests)
