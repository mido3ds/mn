include(CMakeParseArguments)

function(mn_add_plugin)
	cmake_parse_arguments(
		MAP_ARG # prefix of output variables
		"" # list of names of boolean flags (defined ones will be true)
		"NAME;NAMESPACE;UNITY_BUILD;PLUGIN_INTERFACE;EXPORT_MACRO_NAME" # single valued arguments
		"HEADER_FILES;SOURCE_FILES;NATVIS_FILES;WINDOWS_FILES;LINUX_FILES;MACOS_FILES" # list values arguments
		${ARGN} # the argument that will be parsed
	)

	if (WIN32)
		set(OS_SPECIFIC_FILES
			${MAP_ARG_WINDOWS_FILES}
		)
		set(OS_DLL_SUFFIX "dll")
	elseif(UNIX AND NOT APPLE)
		set(OS_SPECIFIC_FILES
			${MAP_ARG_LINUX_FILES}
		)
		set(OS_DLL_SUFFIX "so")
	else if (APPLE)
		set(OS_SPECIFIC_FILES
			${MAP_ARG_MACOS_FILES}
		)
		set(OS_DLL_SUFFIX "dylib")
	endif()

	add_library(${MAP_ARG_NAME}
		SHARED
		${MAP_ARG_HEADER_FILES}
		${MAP_ARG_SOURCE_FILES}
		${MAP_ARG_NATVIS_FILES}
		${CMAKE_CURRENT_SOURCE_DIR}/include/${MAP_ARG_NAME}/Exports.h
		${OS_SPECIFIC_FILES}
	)

	if (NOT ${MAP_ARG_NAMESPACE} STREQUAL "")
		add_library(${MAP_ARG_NAMESPACE}::${MAP_ARG_NAME} ALIAS ${MAP_ARG_NAME})
	endif()

	if (${MAP_ARG_UNITY_BUILD})
		set_target_properties(${MAP_ARG_NAME}
			PROPERTIES
				UNITY_BUILD_BATCH_SIZE 0
				UNITY_BUILD true
		)
	endif(${MAP_ARG_UNITY_BUILD})

	# enable C++17
	# disable any compiler specifc extensions
	target_compile_features(${MAP_ARG_NAME} PUBLIC cxx_std_17)
	set_target_properties(${MAP_ARG_NAME} PROPERTIES
		CXX_EXTENSIONS OFF
		DEBUG_POSTFIX d
	)

	# define debug macro
	target_compile_definitions(${MAP_ARG_NAME} PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

	target_compile_definitions(${MAP_ARG_NAME}
		PUBLIC
		${MAP_ARG_NAME}_PLUGIN="${MAP_ARG_NAME}$<$<CONFIG:DEBUG>:d>.${OS_DLL_SUFFIX}"
	)

	# generate exports header file
	include(GenerateExportHeader)
	if (NOT ${MAP_ARG_EXPORT_MACRO_NAME} STREQUAL "")
		generate_export_header(${MAP_ARG_NAME}
			EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/${MAP_ARG_NAME}/Exports.h
			EXPORT_MACRO_NAME ${MAP_ARG_EXPORT_MACRO_NAME}
		)
	else()
		generate_export_header(${MAP_ARG_NAME}
			EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/${MAP_ARG_NAME}/Exports.h
		)
	endif()

	# list include directories
	target_include_directories(${MAP_ARG_NAME}
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
	)

	if (NOT ${MAP_ARG_PLUGIN_INTERFACE} STREQUAL "")
		message(STATUS "Plugin Interface for '${MAP_ARG_NAME}' is '${MAP_ARG_NAME}-plugin'")
		add_library(${MAP_ARG_NAME}-plugin INTERFACE)

		if (NOT ${MAP_ARG_NAMESPACE} STREQUAL "")
			add_library(${MAP_ARG_NAMESPACE}::${MAP_ARG_NAME}-plugin ALIAS ${MAP_ARG_NAME}-plugin)
		endif()

		add_dependencies(${MAP_ARG_NAME}-plugin ${MAP_ARG_NAME})
		target_include_directories(${MAP_ARG_NAME}-plugin
			INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/${MAP_ARG_PLUGIN_INTERFACE}
		)
	endif()
endfunction(mn_add_plugin)