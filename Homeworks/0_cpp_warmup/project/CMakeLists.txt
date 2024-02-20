cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(CppPractices VERSION 1.0)

set(OUTPUT_PROP
add_subdirectory(src)
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)

add_subdirectory(src)
