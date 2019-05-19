#include "mn/memory/CLib.h"

#include <stdlib.h>

namespace mn::memory
{
	Block
	CLib::alloc(size_t size, uint8_t)
	{
		Block res{};
		res.ptr = ::malloc(size);
		res.size = res.ptr ? size : 0;
		return res;
	}

	void
	CLib::free(Block block)
	{
		::free(block.ptr);
	}

	CLib*
	clib()
	{
		static CLib _clib_allocator;
		return &_clib_allocator;
	}
}
