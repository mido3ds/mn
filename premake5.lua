require "export-compile-commands"

workspace "mn"
	configurations {"debug", "release"}
	platforms {"x86", "x64"}
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}/"
	startproject "unittest"
	defaultplatform "x64"

	if _ACTION then
		location ("projects/" .. _ACTION)
	end

	include "mn"
	include "unittest"