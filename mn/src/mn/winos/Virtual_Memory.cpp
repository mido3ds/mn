#include "mn/Virtual_Memory.h"
#include "mn/Memory.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>

namespace mn
{
	Block
	virtual_alloc(void* address_hint, size_t size)
	{
		Block result{};
		result.ptr = VirtualAlloc(address_hint, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		if(result.ptr)
			result.size = size;
		return result;
	}

	void
	virtual_free(Block block)
	{
		[[maybe_unused]] auto result = VirtualFree(block.ptr, 0, MEM_RELEASE);
		assert(result != NULL);
	}
}
