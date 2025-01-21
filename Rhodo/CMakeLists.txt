# CMake: RhodoEngine->Rhodo

project(Rhodo)

# Recursively find all .cpp and .h files in the src directory
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.h
)
# Now configure spdlog to use external fmt
set(SPDLOG_FMT_EXTERNAL OFF)

## Include the fmtlib.cmake file for fmt configuration
#include(${CMAKE_SOURCE_DIR}/Rhodo/extern/spdlog/cmake/fmtlib.cmake)

# Add the spdlog subdirectory after fmt is available
add_subdirectory(${CMAKE_SOURCE_DIR}/Rhodo/extern/spdlog)

# Define the Rhodo dynamic library
add_library(${PROJECT_NAME} SHARED ${SOURCES} include/Rhodo.h)

# Add include directories for Rhodo
target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/extern/spdlog/include  # Include spdlog
        #        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/extern/fmt/include     # Include fmt
        INTERFACE include
)

# Add defines for Rhodo
target_compile_definitions(${PROJECT_NAME} PRIVATE RH_BUILD_SHARED SPDLOG_COMPILED_LIB)

# Link fmt and spdlog to Rhodo
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

# Custom output paths for Rhodo
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${PLATFORM}/${PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/${PLATFORM}/${PROJECT_NAME}
)

