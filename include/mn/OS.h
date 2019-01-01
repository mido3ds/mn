#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Str.h"
#include "mn/IO.h"

namespace mn
{
	MS_FWD_HANDLE(Allocator);

	/**
	 * @brief      Returns a virtual memory allocator
	 */
	API_MN Allocator
	virtual_allocator();

	/**
	 * @brief      Returns a leak detector allocator
	 */
	API_MN Allocator
	leak_detector();

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
		Stream cause = stream_memory_new(allocator_tmp());
		vprintf(cause, fmt, std::forward<TArgs>(args)...);
		_panic(stream_str(cause));
	}

	/**
	 * @brief      A Helper function which given the filename will load the content of it
	 * into the resulting string
	 *
	 * @param[in]  filename   The filename
	 * @param[in]  allocator  The allocator to be used by the resulting string
	 *
	 * @return     A String containing the content of the file
	 */
	API_MN Str
	file_content_str(const char* filename, Allocator allocator = allocator_top());
}