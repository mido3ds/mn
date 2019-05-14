libmn = {}
libmn.path = path.getabsolute(".")

function libmn.use()
	includedirs "%{libmn.path}/include"
	links "mn"

	filter "configurations:*Static"
		staticruntime "On"

	filter "configurations:debug or release"
		defines {"MN_DLL=0"}

	filter "system:linux"
		linkoptions {"-pthread"}

	filter {}
end

project "mn"
	language "C++"

	files
	{
		"include/**.h",
		"src/mn/*.cpp"
	}

	includedirs "include"

	filter "configurations:debugStatic or releaseStatic"
		staticruntime "On"
		kind "StaticLib"

	filter "configurations:debug or release"
		defines {"MN_DLL=1"}
		kind "SharedLib"

	--linux configuration
	filter "system:linux"
		linkoptions {"-pthread"}
		files {"src/mn/linux/**.cpp"}

	filter { "system:linux", "configurations:debug*" }
		linkoptions {"-rdynamic"}


	--windows configuration
	filter "system:windows"
		files {"src/mn/winos/**.cpp"}

	filter { "system:windows", "configurations:debug*" }
		links {"dbghelp"}

	filter { "system:windows", "action:vs*"}
		files {"tools/vs/mn.natvis"}

return libmn