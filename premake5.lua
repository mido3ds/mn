-- require "export-compile-commands"
workspace "mn"
	configurations {"debug", "release"}
	platforms {"static", "shared"}
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}/"
	location "build"
	-- location ("projects/" .. _ACTION)
	startproject "unittest"

	--language configuration
	warnings "Extra"
	cppdialect "c++17"
	systemversion "latest"
	architecture "x64"

	--linux configuration
	filter "system:linux"
		defines { "OS_LINUX" }

	--windows configuration
	filter "system:windows"
		defines { "OS_WINDOWS" }

	--os agnostic configuration
	filter {"configurations:debug", "kind:SharedLib or StaticLib"}
		targetsuffix "d"

	filter "configurations:debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "NDEBUG" }
		symbols "Off"
		optimize "On"

	include "mn"
	include "unittest"