#include "mn/Memory.h"
#include "mn/Buf.h"
#include "mn/memory/Leak.h"

#include <assert.h>

#if DEBUG
	#define ENABLE_LEAK_DETECTOR 1
#else
	#define ENABLE_LEAK_DETECTOR 0
#endif

namespace mn
{
	//allocator stack interface
	struct Allocator_Stack
	{
		inline static constexpr int CAPACITY = 1024;
		Allocator _stack[CAPACITY];
		int _index = -1;
	};

	inline static Allocator_Stack&
	_allocator_stack()
	{
		thread_local Allocator_Stack stack;
		return stack;
	}

	Allocator
	allocator_top()
	{
		int index = _allocator_stack()._index;
		if (index < 0)
		{
			#if ENABLE_LEAK_DETECTOR
				return memory::leak();
			#else
				return memory::clib();
			#endif
		}

		return _allocator_stack()._stack[index];
	}

	void
	allocator_push(Allocator allocator)
	{
		Allocator* stack = _allocator_stack()._stack;
		int& index = _allocator_stack()._index;
		assert(index < (Allocator_Stack::CAPACITY - 1));
		stack[++index] = allocator;
	}

	void
	allocator_pop()
	{
		int& index = _allocator_stack()._index;
		assert(index >= 0);
		--index;
	}
}
