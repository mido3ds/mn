require "export-compile-commands"

workspace "mn"
	configurations {"debug", "release", "debugStatic", "releaseStatic"}
	platforms {"x86", "x64"}
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}/"
	startproject "unittest"
	defaultplatform "x64"

	if _ACTION then
		location ("projects/" .. _ACTION)
	end

	--language configuration
	warnings "Extra"
	cppdialect "c++17"
	systemversion "latest"

	--linux configuration
	filter "system:linux"
		defines { "OS_LINUX" }

	--windows configuration
	filter "system:windows"
		defines { "OS_WINDOWS" }

	--os agnostic configuration
	filter {"configurations:debug*", "kind:SharedLib or StaticLib"}
		targetsuffix "d"

	filter "configurations:debug*"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release*"
		defines { "NDEBUG" }
		symbols "Off"
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"

	include "mn"
	include "unittest"