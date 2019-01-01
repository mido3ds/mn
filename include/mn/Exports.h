#pragma once

#if defined(OS_WINDOWS)
	#if defined(MN_DLL)
		#define API_MN __declspec(dllexport)
	#else
		#define API_MN __declspec(dllimport)
	#endif
#elif defined(OS_LINUX)
	#define API_MN 
#endif