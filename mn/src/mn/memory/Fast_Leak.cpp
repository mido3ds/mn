#include "mn/memory/Fast_Leak.h"

#include <stdio.h>

namespace mn::memory
{
	Fast_Leak::Fast_Leak()
	{
		this->atomic_size = 0;
		this->atomic_count = 0;
	}

	Fast_Leak::~Fast_Leak()
	{
		if(this->atomic_count > 0)
		{
			::fprintf(
				stderr,
				"Leaks count: %zu, Leaks size(bytes): %zu, for callstack turn on 'MN_LEAK' flag\n",
				atomic_count,
				atomic_size
			);
		}
	}

	Block
	Fast_Leak::alloc(size_t size, uint8_t)
	{
		Block res {::malloc(size), size};
		if(res)
		{
			atomic_inc(atomic_count);
			atomic_add(atomic_size, size);
			return res;
		}
		return {};
	}

	void
	Fast_Leak::free(Block block)
	{
		if(block)
		{
			atomic_dec(atomic_count);
			atomic_add(atomic_size, -int64_t(block.size));
		}
		::free(block.ptr);
	}

	Fast_Leak*
	fast_leak()
	{
		static Fast_Leak _leak_allocator;
		return &_leak_allocator;
	}
}