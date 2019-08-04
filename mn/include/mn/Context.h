#pragma once

#include "mn/Exports.h"

#include <stddef.h>

namespace mn
{
	namespace memory
	{
		struct Interface;
		struct Arena;
	}

	using Allocator = memory::Interface*;

	typedef struct IMemory_Stream* Memory_Stream;
	typedef struct IReader* Reader;

	struct Context
	{
		//allocators data
		inline static constexpr size_t ALLOCATOR_CAPACITY = 1024;
		Allocator _allocator_stack[ALLOCATOR_CAPACITY];
		size_t _allocator_stack_count;

		//tmp allocator
		memory::Arena* _allocator_tmp;

		//Local tmp stream
		Memory_Stream _stream_tmp;
		Reader reader_tmp;
	};

	MN_EXPORT void
	context_init(Context* self);

	MN_EXPORT void
	context_free(Context* self);

	inline static void
	destruct(Context* self)
	{
		context_free(self);
	}

	MN_EXPORT Context*
	context_local(Context* new_context = nullptr);

	/**
	 * Allocators are organized in a per thread stack so that you can change the allocator of any
	 * part of the code even if it doesn't support custom allocator by using
	 * allocator_push() and allocator_pop()
	 * At the base of the stack is the clib_allocator and it can't be popped
	 *
	 * @return     The current top of allocator stack of the calling thread
	 */
	MN_EXPORT Allocator
	allocator_top();

	/**
	 * @brief      Pushes the given allocator to the top of the calling thread allocator stack
	 */
	MN_EXPORT void
	allocator_push(Allocator allocator);

	/**
	 * @brief      Pops an allocator off the calling thread allocator stack
	 */
	MN_EXPORT void
	allocator_pop();

	namespace memory
	{
		MN_EXPORT Arena*
		tmp();
	}

	MN_EXPORT Memory_Stream
	stream_tmp();

	MN_EXPORT Reader
	reader_tmp();
}
