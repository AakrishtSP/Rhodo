add_library(RhodoLogger STATIC)

include(${CMAKE_SOURCE_DIR}/thirdparty/Quill/Quill.cmake)

target_sources(RhodoLogger
PUBLIC
    FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
            Logger.cppm
)

target_link_libraries(RhodoLogger
PRIVATE
    quill::quill
)

target_include_directories(RhodoLogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
