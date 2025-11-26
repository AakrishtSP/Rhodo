# tests/CMakeLists.txt
project(RhodoTests LANGUAGES C CXX)

include(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/Catch2.cmake)

add_subdirectory(Application)
add_subdirectory(Rhodo)


add_executable(RhodoTests)


target_link_libraries(RhodoTests
        PRIVATE
        TestsApplication
        TestsRhodo
        Rhodo
        Catch2::Catch2WithMain   # includes main()
)


set_target_output_dirs(RhodoTests)


include(CTest)
include(Catch)
catch_discover_tests(RhodoTests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


set(TEST_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
if(EXISTS ${TEST_ASSETS})
    add_custom_command(TARGET RhodoTests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${TEST_ASSETS} $<TARGET_FILE_DIR:RhodoTests>/assets
            COMMENT "Copying test assets..."
    )
endif()