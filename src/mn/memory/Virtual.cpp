#include "mn/memory/Virtual.h"
#include "mn/Virtual_Memory.h"

namespace mn::memory
{
	Block
	Virtual::alloc(size_t size, uint8_t)
	{
		Block res = virtual_alloc(nullptr, size);
		return res;
	}

	void
	Virtual::free(Block block)
	{
		virtual_free(block);
	}

	Virtual*
	virtual_mem()
	{
		static Virtual _virtual_allocator;
		return &_virtual_allocator;
	}
}
