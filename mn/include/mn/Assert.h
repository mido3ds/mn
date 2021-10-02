#pragma once

#include "mn/Exports.h"

#if MN_COMPILER_MSVC
	#define mn_debug_break() __debugbreak()
#elif MN_COMPILER_CLANG || MN_COMPILER_GNU
	#define mn_debug_break() __builtin_trap()
#else
	#error unknown compiler
#endif

namespace mn
{
	MN_EXPORT void
	_report_assert_message(const char* expr, const char* message, const char* file, int line);
}

#ifdef NDEBUG
	#define mn_assert(expr, message) ((void)0)
#else
	#define mn_assert(expr, message) { if (expr) {} else { mn::_report_assert_message(#expr, message, __FILE__, __LINE__); mn_debug_break(); } }
#endif
