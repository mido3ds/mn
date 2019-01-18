workspace "mn"
	configurations {"debug", "release"}
	platforms {"x86", "x64"}
	location "build"
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}/"
	startproject "unittest"
	defaultplatform "x64"

	include "mn"
	include "unittest"