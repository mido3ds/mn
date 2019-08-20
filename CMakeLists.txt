cmake_minimum_required(VERSION 3.9)

project(mn VERSION 2.0.3 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_subdirectory(mn)
add_subdirectory(unittest)