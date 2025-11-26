include(FetchContent)

# Avoid re-fetching if already defined
if(NOT TARGET quill)
    FetchContent_Declare(
            quill
            GIT_REPOSITORY https://github.com/odygrd/quill.git
            GIT_TAG v11.0.1
    )
    FetchContent_MakeAvailable(quill)
endif()
