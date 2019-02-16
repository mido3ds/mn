cmake_minimum_required(VERSION 3.10)

project(mn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

# Output directory specification
if(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/x64/$<CONFIG>)
elseif(${CMAKE_SIZEOF_VOID_P} MATCHES 4)
	set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/x86/$<CONFIG>)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
file(GLOB_RECURSE SRC src/*.cpp)

if(WIN32)
	add_definitions(/DOS_WINDOWS=1 /DUNICODE=1 /D_UNICODE=1)
	set(SRC ${SRC} ./tools/vs/mn.natvis)
elseif(UNIX)
	add_definitions(-DOS_LINUX=1)
endif()

add_library(${PROJECT_NAME} SHARED ${SRC})

target_compile_definitions(${PROJECT_NAME}
	PRIVATE MN_DLL=1
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)

option(MN_UNITTEST "Enables mn unittest building" ON)

if(MN_UNITTEST)
	add_subdirectory(unittest)
endif()