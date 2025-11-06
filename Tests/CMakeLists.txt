# tests/CMakeLists.txt
project(RhodoTests LANGUAGES C CXX)

# ------------------------------------------------------------------
# Fetch Catch2
# ------------------------------------------------------------------
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.11.0
        GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(Catch2)

# ------------------------------------------------------------------
# Gather test sources
# ------------------------------------------------------------------
file(GLOB_RECURSE TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# ------------------------------------------------------------------
# Test executable
# ------------------------------------------------------------------
add_executable(RhodoTests ${TEST_SOURCES})


target_link_libraries(RhodoTests
        PRIVATE
        Rhodo
        Catch2::Catch2WithMain   # includes main()
)

target_compile_features(RhodoTests PRIVATE cxx_std_20)

# ------------------------------------------------------------------
# Output directories
# ------------------------------------------------------------------
set_target_output_dirs(RhodoTests)

# ------------------------------------------------------------------
# Enable testing
# ------------------------------------------------------------------
include(CTest)
include(Catch)
catch_discover_tests(RhodoTests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# ------------------------------------------------------------------
# Optional: Copy test assets
# ------------------------------------------------------------------
set(TEST_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
if(EXISTS ${TEST_ASSETS})
    add_custom_command(TARGET RhodoTests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${TEST_ASSETS} $<TARGET_FILE_DIR:RhodoTests>/assets
            COMMENT "Copying test assets..."
    )
endif()