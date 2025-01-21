project(Rhodo)

# Recursively find all .cpp and .h files in the src directory
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.h
)

# Define the Rhodo dynamic library
add_library(${PROJECT_NAME} SHARED ${SOURCES} include/Rhodo.h)

# Add include directories
target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC extern
        PUBLIC extern/spdlog/include
        INTERFACE include
)

# Add defines for Rhodo
target_compile_definitions(${PROJECT_NAME} PRIVATE RH_BUILD_SHARED)

# Custom output paths for Rhodo
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${PLATFORM}/${PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
)
