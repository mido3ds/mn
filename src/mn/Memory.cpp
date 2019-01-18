#include "mn/Memory.h"
#include "mn/Thread.h"
#include "mn/IO.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <atomic>

//to supress warnings in release mode
#define UNUSED(x) ((void)(x))

namespace mn
{
	struct Arena_Node
	{
		Block		mem;
		char*		alloc_head;
		Arena_Node*	next;
	};

	struct Internal_Allocator
	{
		enum KIND
		{
			KIND_NONE,
			KIND_STACK,
			KIND_ARENA,
			KIND_CUSTOM
		};

		KIND kind;
		intptr_t next;

		union
		{
			struct
			{
				Allocator meta_allocator;
				Block memory;
				char* alloc_head;
				size_t allocations_count;
			} stack;

			struct
			{
				Allocator meta_allocator;
				Arena_Node* node_head;
				size_t block_size, total_size, used_size, highwater;
			} arena;

			struct
			{
				void*		self;
				Alloc_Func	alloc;
				Free_Func	free;
			} custom;
		};
	};

	constexpr intptr_t ALLOCATORS_MAX = 4096;
	static Internal_Allocator _allocators[ALLOCATORS_MAX];
	static intptr_t _allocators_head = -1;
	static intptr_t _allocators_count = 0;

	inline static intptr_t
	allocator_get()
	{
		Mutex mtx = _allocators_mutex();
		mutex_lock(mtx);

		static bool allocators_initialized = false;
		if (allocators_initialized == false)
		{
			for (intptr_t i = 0; i < ALLOCATORS_MAX; ++i)
				_allocators[i].next = i + 1;
			_allocators[ALLOCATORS_MAX - 1].next = _allocators_head;
			_allocators_head = 0;
			allocators_initialized = true;
		}

		intptr_t result = _allocators_head;

		if (_allocators_head >= 0)
		{
			assert(_allocators_head < ALLOCATORS_MAX);
			_allocators_head = _allocators[_allocators_head].next;
			++_allocators_count;
		}

		mutex_unlock(mtx);

		return result;
	}

	inline static void
	allocator_put(intptr_t ix)
	{
		Mutex mtx = _allocators_mutex();
		mutex_lock(mtx);

		assert(ix < ALLOCATORS_MAX && ix >= 0);
		_allocators[ix].next = _allocators_head;
		_allocators_head = ix;
		--_allocators_count;

		mutex_unlock(mtx);
	}


	//custom allocator interface
	Allocator
	allocator_stack_new(size_t stack_size, Allocator meta_allocator)
	{
		assert(stack_size != 0);
		intptr_t ix = allocator_get();
		assert(ix != -1 &&
			"you have exhausted the allocator pool. you cannot have more than 4096 allocators at a time");
		Internal_Allocator* self = _allocators + ix;
		self->kind = Internal_Allocator::KIND_STACK;
		self->stack.meta_allocator = meta_allocator;
		self->stack.memory = alloc_from(self->stack.meta_allocator, stack_size, alignof(int));
		self->stack.alloc_head = (char*)self->stack.memory.ptr;
		self->stack.allocations_count = 0;
		return (Allocator)intptr_t(ix);
	}

	void
	allocator_stack_free_all(Allocator allocator)
	{
		intptr_t ix = (intptr_t)allocator;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			"Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		assert(self->kind == Internal_Allocator::KIND_STACK &&
			"provided allocator is not a stack allocator");

		self->stack.alloc_head = (char*)self->stack.memory.ptr;
	}

	Allocator
	allocator_arena_new(size_t block_size, Allocator meta_allocator)
	{
		assert(block_size != 0);
		intptr_t ix = allocator_get();
		assert(ix != -1 &&
			"you have exhausted the allocator pool. you cannot have more than 4096 allocators at a time");
		Internal_Allocator* self = _allocators + ix;
		self->kind = Internal_Allocator::KIND_ARENA;
		self->arena.meta_allocator = meta_allocator;
		self->arena.node_head = nullptr;
		self->arena.block_size = block_size;
		self->arena.total_size = 0;
		self->arena.used_size = 0;
		self->arena.highwater = 0;
		return (Allocator)ix;
	}

	void
	allocator_arena_grow(Allocator arena, size_t grow_size)
	{
		intptr_t ix = (intptr_t)arena;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			"Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		assert(self->kind == Internal_Allocator::KIND_ARENA &&
			"provided allocator is not an arena allocator");

		if (self->arena.node_head == nullptr ||
			(self->arena.node_head->mem.size - (self->arena.node_head->alloc_head - (char*)self->arena.node_head->mem.ptr)) < grow_size)
		{
			size_t node_size = sizeof(Arena_Node);
			size_t request_size = grow_size > self->arena.block_size ? grow_size : self->arena.block_size;
			request_size += node_size;

			Arena_Node* new_node = (Arena_Node*)alloc_from(self->arena.meta_allocator, request_size, alignof(int)).ptr;
			self->arena.total_size += request_size - node_size;

			new_node->mem.ptr = &new_node[1];
			new_node->mem.size = request_size - node_size;
			new_node->alloc_head = (char*)new_node->mem.ptr;
			new_node->next = self->arena.node_head;
			self->arena.node_head = new_node;
		}
	}

	void
	allocator_arena_free_all(Allocator arena)
	{
		intptr_t ix = (intptr_t)arena;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			"Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		assert(self->kind == Internal_Allocator::KIND_ARENA &&
			"provided allocator is not an arena allocator");

		while (self->arena.node_head)
		{
			auto next = self->arena.node_head->next;
			Block block{ self->arena.node_head, self->arena.node_head->mem.size + sizeof(Arena_Node) };
			free_from(self->arena.meta_allocator, block);
			self->arena.node_head = next;
		}
		self->arena.node_head = nullptr;
		self->arena.total_size = 0;
		self->arena.used_size = 0;
	}

	size_t
	allocator_arena_used_size(Allocator arena)
	{
		intptr_t ix = (intptr_t)arena;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			"Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		assert(self->kind == Internal_Allocator::KIND_ARENA &&
			"provided allocator is not an arena allocator");

		return self->arena.used_size;
	}

	size_t
	allocator_arena_highwater(Allocator arena)
	{
		intptr_t ix = (intptr_t)arena;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			"Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		assert(self->kind == Internal_Allocator::KIND_ARENA &&
			"provided allocator is not an arena allocator");

		return self->arena.highwater;
	}

	Allocator
	allocator_custom_new(void* custom_self, Alloc_Func custom_alloc, Free_Func custom_free)
	{
		intptr_t ix = allocator_get();
		assert(ix != -1 &&
			   "you have exhausted the allocator pool. you cannot have more than 4096 allocators at a time");
		Internal_Allocator* self = _allocators + ix;
		self->kind = Internal_Allocator::KIND_CUSTOM;
		self->custom.self = custom_self;
		self->custom.alloc = custom_alloc;
		self->custom.free = custom_free;
		return (Allocator)ix;
	}

	void
	allocator_free(Allocator allocator)
	{
		intptr_t ix = (intptr_t)allocator;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			   "Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		switch(self->kind)
		{
			case Internal_Allocator::KIND_STACK:
				free_from(self->stack.meta_allocator, self->stack.memory);
				break;

			case Internal_Allocator::KIND_ARENA:
			{
				while(self->arena.node_head)
				{
					auto next = self->arena.node_head->next;
					Block block { self->arena.node_head, self->arena.node_head->mem.size + sizeof(Arena_Node) };
					free_from(self->arena.meta_allocator, block);
					self->arena.node_head = next;
				}
				self->arena.total_size = 0;
				self->arena.used_size = 0;
				break;
			}

			case Internal_Allocator::KIND_CUSTOM:
			{
				//nothing to do here
				break;
			}

			case Internal_Allocator::KIND_NONE:
			default:
				assert(false && "Invalid allocator handle");
				break;
		}
		allocator_put(ix);
	}
	
	//allocator stack interface
	constexpr size_t ALLOCATOR_STACK_SIZE = 1024;
	thread_local Allocator _allocator_stack[ALLOCATOR_STACK_SIZE];
	thread_local size_t _allocator_stack_count = 0;

	struct Allocator_Tmp_Alert
	{
		Allocator _tmp;

		Allocator_Tmp_Alert()
		{
			_tmp = allocator_arena_new(4096, clib_allocator);
		}

		~Allocator_Tmp_Alert()
		{
			#if DEBUG
				printfmt_err("Temp Allocator 0x{:X}: {} bytes used at exit, {} bytes highwater mark\n",
					_tmp, allocator_arena_used_size(_tmp), allocator_arena_highwater(_tmp));
			#endif
			allocator_free(_tmp);
		}
	};

	Allocator
	allocator_tmp()
	{
		thread_local Allocator_Tmp_Alert _tmp_alert;
		return _tmp_alert._tmp;
	}

	void
	allocator_tmp_free()
	{
		allocator_arena_free_all(allocator_tmp());
	}

	size_t
	allocator_tmp_used_size()
	{
		return allocator_arena_used_size(allocator_tmp());
	}

	size_t
	allocator_tmp_highwater()
	{
		return allocator_arena_highwater(allocator_tmp());
	}

	Allocator
	allocator_top()
	{
		if(_allocator_stack_count == 0)
			return clib_allocator;

		assert(_allocator_stack_count <= ALLOCATOR_STACK_SIZE);
		return _allocator_stack[_allocator_stack_count - 1];
	}

	void
	allocator_push(Allocator allocator)
	{
		assert(_allocator_stack_count < ALLOCATOR_STACK_SIZE);
		_allocator_stack[_allocator_stack_count] = allocator;
		++_allocator_stack_count;
	}

	void
	allocator_pop()
	{
		assert(_allocator_stack_count > 0);
		--_allocator_stack_count;
	}

	#define ENABLE_SIMPLE_LEAK_DETECTION 0

	#if ENABLE_SIMPLE_LEAK_DETECTION
	struct Simple_Leak_Detector
	{
		std::atomic<size_t> allocation_count = 0;
		std::atomic<size_t> allocation_size = 0;

		~Simple_Leak_Detector()
		{
			size_t ac = allocation_count.load();
			size_t as = allocation_size.load();
			if(ac > 0)
			{
				::fprintf(stderr, "[[LEAK REPORT]]\nallocation_count: %zu\nallocation_size: %zu\n",
						  ac, as);
			}
		}
	};
	static Simple_Leak_Detector _simple_leak_detector;
	#endif

	//alloc from interface
	Block
	alloc_from(Allocator allocator, size_t size, uint8_t alignment)
	{
		if(allocator == clib_allocator)
		{
			void* ptr = ::malloc(size);

			#if ENABLE_SIMPLE_LEAK_DETECTION
			if(ptr)
			{
				++_simple_leak_detector.allocation_count;
				_simple_leak_detector.allocation_size += size;
			}
			#endif
			return Block { ptr, ptr ? size : 0 };
		}

		//check for type and act accordingly
		intptr_t ix = (intptr_t)allocator;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			   "Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		switch(self->kind)
		{
			case Internal_Allocator::KIND_STACK:
			{
				ptrdiff_t used_memory = self->stack.alloc_head - (char*)self->stack.memory.ptr;
				size_t free_memory = self->stack.memory.size - used_memory;
				assert(free_memory >= size &&
					   "Stack allocator doesn't have enough memory");
				UNUSED(free_memory);

				char* ptr = self->stack.alloc_head;
				self->stack.alloc_head = ptr + size;
				self->stack.allocations_count++;
				return Block { ptr, size };
			}

			case Internal_Allocator::KIND_ARENA:
			{
				allocator_arena_grow(allocator, size);

				char* ptr = self->arena.node_head->alloc_head;
				self->arena.node_head->alloc_head += size;
				self->arena.used_size += size;
				self->arena.highwater = self->arena.highwater > self->arena.used_size ? self->arena.highwater : self->arena.used_size;

				return Block{ ptr, size };
			}

			case Internal_Allocator::KIND_CUSTOM:
			{
				if(self->custom.alloc)
					return self->custom.alloc(self->custom.self, size, alignment);
				break;
			}

			case Internal_Allocator::KIND_NONE:
			default:
				assert(false && "Invalid allocator handle");
				break;
		}
		return Block{};
	}

	void
	free_from(Allocator allocator, Block block)
	{
		if(allocator == clib_allocator)
		{
			#if ENABLE_SIMPLE_LEAK_DETECTION
			if(block.ptr)
			{
				--_simple_leak_detector.allocation_count;
				_simple_leak_detector.allocation_size -= block.size;
			}
			#endif
			::free(block.ptr);
			return;
		}

		intptr_t ix = (intptr_t)allocator;
		assert(ix >= 0 && ix < ALLOCATORS_MAX &&
			   "Invalid allocator handle");

		Internal_Allocator* self = _allocators + ix;
		switch(self->kind)
		{
			case Internal_Allocator::KIND_STACK:
			{
				assert(self->stack.allocations_count > 0);
				--self->stack.allocations_count;
				if(self->stack.allocations_count == 0)
					self->stack.alloc_head = (char*)self->stack.memory.ptr;

				break;
			}

			case Internal_Allocator::KIND_ARENA:
			{
				self->arena.used_size -= block.size;
				break;
			}

			case Internal_Allocator::KIND_CUSTOM:
			{
				if(self->custom.free)
					self->custom.free(self->custom.self, block);
				break;
			}

			case Internal_Allocator::KIND_NONE:
			default:
				assert(false && "Invalid allocator handle");
				break;
		}
	}
}