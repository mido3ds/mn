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
		int _count = 0;
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
		Allocator_Stack& self = _allocator_stack();
		if (self._count == 0)
		{
			#if ENABLE_LEAK_DETECTOR
				return memory::leak();
			#else
				return memory::clib();
			#endif
		}

		return self._stack[self._count - 1];
	}

	void
	allocator_push(Allocator allocator)
	{
		Allocator_Stack& self = _allocator_stack();
		assert(self._count < Allocator_Stack::CAPACITY);
		self._stack[self._count++] = allocator;
	}

	void
	allocator_pop()
	{
		Allocator_Stack& self = _allocator_stack();
		assert(self._count > 0);
		--self._count;
	}
}
