cmake_minimum_required(VERSION 3.10)

# list source files
set(SOURCE_FILES
	src/unittest_mn.cpp
	src/unittest_main.cpp
)

# add executable target
add_executable(unittest
	${SOURCE_FILES}
)

# add doctest folder
target_include_directories(unittest
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/doctest
		MoustaphaSaad::mn
)