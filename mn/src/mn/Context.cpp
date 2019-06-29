#include "mn/Context.h"
#include "mn/Memory.h"
#include "mn/memory/Leak.h"

#include <assert.h>
#include <stdio.h>

namespace mn
{
	struct Context_Wrapper
	{
		Context self;

		Context_Wrapper()
		{
			context_init(&self);
		}

		~Context_Wrapper()
		{
			#if DEBUG
				fprintf(stderr, "Temp Allocator %p: %zu bytes used at exit, %zu bytes highwater mark\n",
					self._allocator_tmp, self._allocator_tmp->used_mem, self._allocator_tmp->highwater_mem);
			#endif

			context_free(&self);
		}
	};

	inline static Context_Wrapper*
	_context_wrapper()
	{
		thread_local Context_Wrapper _CONTEXT;
		return &_CONTEXT;
	}
	thread_local Context* _CURRENT_CONTEXT = nullptr;


	//API
	void
	context_init(Context* self)
	{
		for (size_t i = 0; i < Context::ALLOCATOR_CAPACITY; ++i)
			self->_allocator_stack[i] = nullptr;

			#if DEBUG
				self->_allocator_stack[0] = memory::leak();
			#else
				self->_allocator_stack[0] = memory::clib();
			#endif
		self->_allocator_stack_count = 1;

		self->_allocator_tmp = alloc_construct_from<memory::Arena>(memory::clib(), 4ULL * 1024ULL * 1024ULL, memory::clib());
	}

	void
	context_free(Context* self)
	{
		free_destruct_from(memory::clib(), self->_allocator_tmp);
	}

	Context*
	context_local(Context* new_context)
	{
		if (_CURRENT_CONTEXT == nullptr)
			_CURRENT_CONTEXT = &_context_wrapper()->self;

		if(new_context == nullptr)
			return _CURRENT_CONTEXT;

		Context* res = _CURRENT_CONTEXT;
		_CURRENT_CONTEXT = new_context;
		return res;
	}

	Allocator
	allocator_top()
	{
		Context* self = context_local();
		return self->_allocator_stack[self->_allocator_stack_count - 1];
	}

	void
	allocator_push(Allocator allocator)
	{
		Context* self = context_local();
		assert(self->_allocator_stack_count < Context::ALLOCATOR_CAPACITY);
		self->_allocator_stack[self->_allocator_stack_count++] = allocator;
	}

	void
	allocator_pop()
	{
		Context* self = context_local();
		assert(self->_allocator_stack_count > 0);
		--self->_allocator_stack_count;
	}

	namespace memory
	{
		Arena*
		tmp()
		{
			return context_local()->_allocator_tmp;
		}
	}
}
