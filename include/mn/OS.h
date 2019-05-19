#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/IO.h"

namespace mn
{
	[[noreturn]] API_MN void
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
		Stream cause = stream_memory_new(memory::tmp());
		vprintf(cause, fmt, std::forward<TArgs>(args)...);
		_panic(stream_str(cause));
	}
}
