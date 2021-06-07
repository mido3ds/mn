#pragma once

#include "mn/Exports.h"
#include "mn/Fmt.h"

namespace mn
{
	[[noreturn]] MN_EXPORT void
	_panic(const char* cause);

	// prints the given message and the call stack then terminates the program
	template<typename ... TArgs>
	[[noreturn]] inline static void
	panic(const char* fmt, TArgs&& ... args)
	{
		_panic(str_tmpf(fmt, args...).ptr);
	}
}
