#pragma once

#include "mn/Exports.h"
#include "mn/Str.h"

namespace mn
{
	/**
	 * @brief      Outputs the current callstack to a string
	 *
	 * @param[in]  allocator  The allocator to be used by the string
	 *
	 * @return     A String containing the callstack
	 */
	API_MN Str
	callstack_dump(Allocator allocator = memory::tmp());
}
