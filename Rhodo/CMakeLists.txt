# CMake: RhodoEngine->Rhodo

project(Rhodo)

# Recursively find all .cpp and .h files in the src directory
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.h
)

# Define the Rhodo dynamic library
#add_library(${PROJECT_NAME} SHARED ${SOURCES} include/Rhodo.h)
add_library(${PROJECT_NAME} STATIC ${SOURCES} include/Rhodo.h)

# Add GLM as a submodule
add_subdirectory(external/glm glm_build)

# Add GLFW
add_subdirectory(external/glfw glfw_build)

# Add Glad2
add_subdirectory(external/glad/cmake glad_build)
glad_add_library(glad_gl STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# Add include directories for Rhodo
target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/spdlog/include  # Include spdlog
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/glm  # Include glm
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/glfw/include  # Include GLFW
        PUBLIC ${CMAKE_SOURCE_DIR}/Rhodo/external/glad/include # Include Glad
        PUBLIC ${CMAKE_BINARY_DIR}/Rhodo/gladsources/glad_gl/include # Include Glad generated files
        INTERFACE include
)

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Rhodo/src/rhpch.h")

target_link_libraries(${PROJECT_NAME}
        PRIVATE glfw
        PRIVATE glad_gl
)

# Add defines for Rhodo
#target_compile_definitions(${PROJECT_NAME} PRIVATE RH_BUILD_SHARED)
target_compile_definitions(${PROJECT_NAME} PRIVATE RH_BUILD_STATIC)

# Custom output paths for Rhodo
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${PLATFORM}/${PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib/${PLATFORM}/${PROJECT_NAME}
)

