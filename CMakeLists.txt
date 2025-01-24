cmake_minimum_required(VERSION 3.30)
project(RhodoEngine)

# Set global properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the base directory as the root of the project
set(BASE_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)

# Platform and configuration variables
set(PLATFORM $<IF:$<PLATFORM_ID:Windows>,Win,$<IF:$<PLATFORM_ID:Linux>,Linux,Unix>>)
set(CONFIGURATION $<CONFIG>)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/build/bin/${CONFIGURATION}-${PLATFORM})

# Add subdirectories for the two projects
add_subdirectory(Rhodo)
add_subdirectory(Examples)



# Apply defines globally based on configuration
add_compile_definitions(
        $<$<CONFIG:Debug>:RH_DEBUG;RH_ENABLE_LOGGING;RH_ENABLE_ASSERTS>          # Debug builds
        $<$<CONFIG:RelWithDebInfo>:RH_RELEASE;RH_ENABLE_LOGGING>                # ReleaseWithLog builds
        $<$<CONFIG:Release>:RH_RELEASE>                                         # Release builds
)