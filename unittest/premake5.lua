project "unittest"
	language "C++"
	kind "ConsoleApp"

	files
	{
		"unittest_main.cpp",
		"unittest_mn.cpp"
	}

	includedirs
	{
		"Catch2/single_include",
		"../include"
	}

	links
	{
		"mn"
	}

	cppdialect "c++17"
	systemversion "latest"

	filter "system:linux"
		defines { "OS_LINUX" }
		linkoptions {"-pthread"}

	filter { "system:linux", "configurations:debug" }
		linkoptions {"-rdynamic"}

	filter "system:windows"
		defines { "OS_WINDOWS" }
		buildoptions {"/utf-8"}

	filter { "system:windows", "configurations:debug" }
		links {"dbghelp"}