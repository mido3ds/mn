#pragma once

#include "mn/Exports.h"
#include "mn/Fmt.h"

namespace mn
{
	[[noreturn]] MN_EXPORT void
	_panic(const char* cause);

	/**
	 * @brief      Prints the given message and the call stack then exits the application
	 *
	 * @param[in]  fmt        The format
	 * @param[in]  args       The arguments
	 */
	template<typename ... TArgs>
	[[noreturn]] inline static void
	panic(const char* fmt, TArgs&& ... args)
	{
		_panic(str_tmpf(fmt, args...).ptr);
	}
}
