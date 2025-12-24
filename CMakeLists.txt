cmake_minimum_required(VERSION 3.15)
option(CPPSP_BUILD_EXAMPLES "Build cppsp examples" OFF)
project(cppsp)
add_executable(cppsp_compiler "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
if(CPPSP_BUILD_EXAMPLES)
add_custom_command(
    TARGET cppsp_compiler
    POST_BUILD
    COMMAND "$<TARGET_FILE:cppsp_compiler>" ../examples/helloworld.cppsp
    COMMAND "$<TARGET_FILE:cppsp_compiler>" ../examples/advanced.cppsp
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Building examples..."
)
endif()
