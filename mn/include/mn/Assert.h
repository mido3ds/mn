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
	#define mn_assert_msg(expr, message) ((void)0)
	#define mn_assert(expr) ((void)0)
#else
	#define mn_assert_msg(expr, message) do { if (expr) {} else { mn::_report_assert_message(#expr, message, __FILE__, __LINE__); mn_debug_break(); } } while(false)
	#define mn_assert(expr) do { if (expr) {} else { mn::_report_assert_message(#expr, nullptr, __FILE__, __LINE__); mn_debug_break(); } } while(false)
#endif

#define mn_unreachable() mn_assert_msg(false, "unreachable")
#define mn_unreachable_msg(message) mn_assert_msg(false, message)
