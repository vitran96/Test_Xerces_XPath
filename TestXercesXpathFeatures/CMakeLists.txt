cmake_minimum_required (VERSION 3.8)

project ("TestXercesXpathFeatures")

# Add source to this project's executable.
add_executable (TestXercesXpathFeatures "TestXercesXpathFeatures.cpp" "TestXercesXpathFeatures.h")

# Set include paths
include_directories(
    #On Windows, xerces config header is saved in cmake cache
    #This line is required to include the config
    ${CMAKE_BINARY_DIR}/lib/xerces-c-3.2.2/src
    ${CMAKE_SOURCE_DIR}/lib/xerces-c-3.2.2/src
)

target_link_libraries(${PROJECT_NAME} xerces-c)

add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory
    ${CMAKE_SOURCE_DIR}/resource
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
