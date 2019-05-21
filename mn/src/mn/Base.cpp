#include "mn/Base.h"

#include <string.h>

namespace mn
{
	void
	block_zero(Block block)
	{
		::memset(block.ptr, 0, block.size);
	}

	Block
	block_lit(const char* str)
	{
		return Block { (void*)str, ::strlen(str) };
	}
}
