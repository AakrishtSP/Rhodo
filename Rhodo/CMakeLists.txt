# CMake: RhodoEngine->Rhodo

project(Rhodo)

# Recursively find all .cpp and .h files in the src directory
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.h
)

# Define the Rhodo dynamic library
add_library(${PROJECT_NAME} SHARED ${SOURCES} include/Rhodo.h)

# Add GLM as a submodule
add_subdirectory(external/glm)

# Add GLFW
add_subdirectory(external/glfw)

# Add include directories for Rhodo
target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/spdlog/include  # Include spdlog
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/glm  # Include glm
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/glfw/include  # Include GLFW
        INTERFACE include
)

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Rhodo/src/rhpch.h")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Add defines for Rhodo
target_compile_definitions(${PROJECT_NAME} PRIVATE RH_BUILD_SHARED)

# Custom output paths for Rhodo
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${PLATFORM}/${PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/${PLATFORM}/${PROJECT_NAME}
)

