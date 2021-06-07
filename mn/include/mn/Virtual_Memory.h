#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

namespace mn
{
	// allocates a block of memory using OS virtual memory, it will commit it as well
	MN_EXPORT Block
	virtual_alloc(void* address_hint, size_t size);

	// frees a block from OS virtual memory
	MN_EXPORT void
	virtual_free(Block block);
}
