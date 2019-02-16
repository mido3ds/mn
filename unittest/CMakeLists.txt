project(unittest LANGUAGES CXX)

add_executable(${PROJECT_NAME}
	unittest_main.cpp
	unittest_mn.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/Catch2/single_include
)

target_link_libraries(${PROJECT_NAME} mn)