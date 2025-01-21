project(Examples)

# Recursively find all .cpp and .h files in the src directory
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.h
)

# Define the Examples executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories for Examples
target_include_directories(${PROJECT_NAME}
        PRIVATE src
)

# Add defines for Examples
target_compile_definitions(${PROJECT_NAME} PRIVATE RH_IMPORT_SHARED)

# Link Rhodo to Examples
target_link_libraries(${PROJECT_NAME} PRIVATE Rhodo)

# Custom output paths for Examples
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/obj/${PLATFORM}/${PROJECT_NAME}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
)

# Set working directory for Examples to be the Examples folder
set_target_properties(${PROJECT_NAME} PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Examples)

# Post-build process to copy Rhodo DLL to Examples build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:Rhodo>
        ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}
)

# Copy assets to the build directory after building Examples
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/assets
        ${CMAKE_BINARY_DIR}/build/bin/${PLATFORM}/${PROJECT_NAME}/assets
)
