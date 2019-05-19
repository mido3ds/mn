libmn = {}
libmn.path = path.getabsolute(".")

function libmn.use()
	includedirs "%{libmn.path}/include"
	links "mn"

	filter "platforms:static"
		staticruntime "On"

	filter "platforms:shared"
		defines {"MN_DLL=0"}

	filter "system:linux"
		linkoptions {"-pthread"}

	filter {}
end

project "mn"
	language "C++"

	files
	{
		"include/mn/*.h",
		"include/mn/memory/*.h",
		"src/mn/*.cpp",
		"src/mn/memory/*.cpp"
	}

	includedirs "include"

	filter "platforms:static"
		staticruntime "On"
		kind "StaticLib"

	filter "platforms:shared"
		defines {"MN_DLL=1"}
		kind "SharedLib"

	--linux configuration
	filter "system:linux"
		linkoptions {"-pthread"}
		files {"src/mn/linux/**.cpp"}

	filter { "system:linux", "configurations:debug" }
		linkoptions {"-rdynamic"}


	--windows configuration
	filter "system:windows"
		files {"src/mn/winos/**.cpp"}

	filter { "system:windows", "configurations:debug" }
		links {"dbghelp"}

	filter { "system:windows", "action:vs*"}
		files {"tools/vs/mn.natvis"}

return libmn