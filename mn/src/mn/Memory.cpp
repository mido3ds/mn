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
		Buf<Allocator> stack;

		Allocator_Stack_Wrapper()
		{
			stack = buf_with_allocator<Allocator>(memory::clib());
		}

		~Allocator_Stack_Wrapper()
		{
			buf_free(stack);
		}
	};

	inline static Buf<Allocator>&
	_allocator_stack()
	{
		thread_local Allocator_Stack_Wrapper _wrapper;
		return _wrapper.stack;
	}

	Allocator
	allocator_top()
	{
		Buf<Allocator>& stack = _allocator_stack();
		if (stack.count == 0)
		{
			#if ENABLE_LEAK_DETECTOR
				return memory::leak();
			#else
				return memory::clib();
			#endif
		}
		return buf_top(stack);
	}

	void
	allocator_push(Allocator allocator)
	{
		Buf<Allocator>& stack = _allocator_stack();
		buf_push(stack, allocator);
	}

	void
	allocator_pop()
	{
		Buf<Allocator>& stack = _allocator_stack();
		buf_pop(stack);
	}
}
