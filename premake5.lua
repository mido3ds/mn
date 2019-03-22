require "export-compile-commands"

workspace "mn"
	configurations {"debug", "release"}
	platforms {"x86", "x64"}
	location ("projects/" .. _ACTION)
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}/"
	startproject "unittest"
	defaultplatform "x64"

	include "mn"
	include "unittest"