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
		"doctest"
	}

	libmn.use()
