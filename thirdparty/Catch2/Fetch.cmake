include(FetchContent)

# Avoid re-fetching if already defined
if(NOT TARGET Catch2)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.11.0
        GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(Catch2)
endif()
