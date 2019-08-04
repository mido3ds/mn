#pragma once

#include "mn/Exports.h"
#include "mn/Memory_Stream.h"
#include "mn/IO.h"

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
		Memory_Stream cause = memory_stream_new(memory::tmp());
		vprintf(cause, fmt, std::forward<TArgs>(args)...);
		_panic(cause->str.ptr);
	}
}
