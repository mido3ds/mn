cmake_minimum_required(VERSION 3.10)

project(mn VERSION 1.0.0 LANGUAGES CXX)

set(HEADER_FILES
	include/mn/memory/Arena.h
	include/mn/memory/CLib.h
	include/mn/memory/Interface.h
	include/mn/memory/Leak.h
	include/mn/memory/Stack.h
	include/mn/memory/Virtual.h
	include/mn/Base.h
	include/mn/Buf.h
	include/mn/Debug.h
	include/mn/Defer.h
	include/mn/Exports.h
	include/mn/File.h
	include/mn/IO.h
	include/mn/Map.h
	include/mn/Memory.h
	include/mn/Memory_Stream.h
	include/mn/OS.h
	include/mn/Pool.h
	include/mn/Reader.h
	include/mn/Ring.h
	include/mn/Str.h
	include/mn/Str_Intern.h
	include/mn/Stream.h
	include/mn/Thread.h
	include/mn/Virtual_Memory.h
)

set(SOURCE_FILES
	src/mn/memory/Arena.cpp
	src/mn/memory/CLib.cpp
	src/mn/memory/Leak.cpp
	src/mn/memory/Stack.cpp
	src/mn/memory/Virtual.cpp
	src/mn/Base.cpp
	src/mn/Memory.cpp
	src/mn/Memory_Stream.cpp
	src/mn/OS.cpp
	src/mn/Pool.cpp
	src/mn/Reader.cpp
	src/mn/Str.cpp
	src/mn/Str_Intern.cpp
	src/mn/Stream.cpp
)

if(WIN32)
	set(SOURCE_FILES ${SOURCE_FILES} 
		src/mn/winos/Debug.cpp
		src/mn/winos/File.cpp
		src/mn/winos/Thread.cpp
		src/mn/winos/Virtual_Memory.cpp
	)
elseif(LINUX)
	set(SOURCE_FILES ${SOURCE_FILES} 
		src/mn/linux/Debug.cpp
		src/mn/linux/File.cpp
		src/mn/linux/Thread.cpp
		src/mn/linux/Virtual_Memory.cpp
	)
endif()

add_library(mn
	${HEADER_FILES}
	${SOURCE_FILES}
)

if(WIN32)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE
		UNICODE=1
		_UNICODE=1
	)
endif(WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
	EXPORT_FILE_NAME ${CMAKE_SOURCE_DIR}/include/mn/Exports.h
	EXPORT_MACRO_NAME API_MN
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	include
)