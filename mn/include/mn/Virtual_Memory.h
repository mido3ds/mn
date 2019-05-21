#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

namespace mn
{
	/**
	 * @brief      Virtual allocates a block of memory
	 *
	 * @param      address_hint  The address hint for the allocator to put the allocated memory at
	 * @param[in]  size          The size(in bytes)
	 */
	MN_EXPORT Block
	virtual_alloc(void* address_hint, size_t size);

	/**
	 * @brief      Virtual frees a virtual allocated block of memory
	 *
	 * @param[in]  block  The block
	 */
	MN_EXPORT void
	virtual_free(Block block);
}
