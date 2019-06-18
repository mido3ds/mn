#include "mn/Memory.h"
#include "mn/Buf.h"
#include "mn/memory/Leak.h"

#if DEBUG
	#define ENABLE_LEAK_DETECTOR 1
#else
	#define ENABLE_LEAK_DETECTOR 0
#endif

namespace mn
{
	//allocator stack interface
	struct Allocator_Stack_Wrapper
	{
		inline static constexpr size_t CAPACITY = 1024;
		Allocator stack[CAPACITY];
		int allocator_index = -1;
	};

	inline static Allocator_Stack_Wrapper&
	_allocator_wrapper()
	{
		thread_local Allocator_Stack_Wrapper _wrapper;
		return _wrapper;
	}

	inline static Allocator*
	_allocator_stack()
	{
		return _allocator_wrapper().stack;
	}

	inline static int&
	_allocator_index()
	{
		return _allocator_wrapper().allocator_index;
	}

	Allocator
	allocator_top()
	{
		int index = _allocator_index();
		if (index < 0)
		{
			#if ENABLE_LEAK_DETECTOR
				return memory::leak();
			#else
				return memory::clib();
			#endif
		}

		return _allocator_stack()[index];
	}

	void
	allocator_push(Allocator allocator)
	{
		Allocator* stack = _allocator_stack();
		int& allocator_index = _allocator_index();
		stack[++allocator_index] = allocator;
	}

	void
	allocator_pop()
	{
		int& allocator_index = _allocator_index();
		--allocator_index;
	}
}
