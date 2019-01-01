#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

#include <stdint.h>

namespace mn
{
	/**
	 * An Allocator is a handle to the underlying type which provides the following
	 * interfaces:
	 * - alloc_from(Allocator allocator, size_t size, uint8_t alignment)
	 * - free_from(Allocator allocator, Block block)
	 */
	MS_HANDLE(Allocator);

	/**
	 * This is the default c runtime library allocator
	 * - malloc
	 * - free
	 */
	inline Allocator clib_allocator = Allocator(-1);


	/**
	 * Every thread has a tmp allocator to enable it to allocate throw-away data structures without
	 * worrying about freeing them or about the owner of such data structure on the premise that
	 * the application will -at a suitable time- call allocator_tmp_free()
	 *
	 * @return     An Allocator handle to the calling thread tmp allocator
	 */
	API_MN Allocator
	allocator_tmp();

	/**
	 * @brief      Frees all the tmp memory of the calling thread
	 */
	API_MN void
	allocator_tmp_free();

	/**
	 * @return     The used size (in bytes) of the calling thread tmp allocator
	 */
	API_MN size_t
	allocator_tmp_used_size();

	/**
	 * @return     The max used size(in bytes) through out the usage of the calling thread tmp allocator
	 */
	API_MN size_t
	allocator_tmp_highwater();

	/**
	 * Allocators are organized in a per thread stack so that you can change the allocator of any 
	 * part of the code even if it doesn't support custom allocator by using
	 * allocator_push() and allocator_pop()
	 * At the base of the stack is the clib_allocator and it can't be popped
	 *
	 * @return     The current top of allocator stack of the calling thread
	 */
	API_MN Allocator
	allocator_top();

	/**
	 * @brief      Pushes the given allocator to the top of the calling thread allocator stack
	 */
	API_MN void
	allocator_push(Allocator allocator);

	/**
	 * @brief      Pops an allocator off the calling thread allocator stack
	 */
	API_MN void
	allocator_pop();


	//custom allocator interface
	/**
	 * Allocation function pointer signature
	 */
	using Alloc_Func = Block(*)(void*, size_t, uint8_t);
	/**
	 * Free function pointer signature
	 */
	using Free_Func  = void(*)(void*, Block);


	/**
	 * @brief      Creates a new fixed-size stack allocator
	 *
	 * @param[in]  stack_size      The stack size
	 * @param[in]  meta_allocator  The meta allocator used by the stack allocator
	 *
	 * @return     The stack allocator handle
	 */
	API_MN Allocator
	allocator_stack_new(size_t stack_size,
						Allocator meta_allocator = allocator_top());

	/**
	 * @brief      Frees all the used memory of a stack allocator thus resetting it to
	 * the start state, as if you just called allocator_stack_new()
	 *
	 * @param[in]  stack  The stack allocator
	 */
	API_MN void
	allocator_stack_free_all(Allocator stack);

	/**
	 * An Arena allocator is just a linked list of stack allocators and when the stack allocator is
	 * at full capacity then it allocates another bucket-stack- and adds it to the arena list
	 * 
	 * @brief      Creates a new dynamic-size arena allocator
	 *
	 * @param[in]  block_size      The block size in which the arena will be allocating
	 * @param[in]  meta_allocator  The meta allocator used by the arena allocator
	 *
	 * @return     { description_of_the_return_value }
	 */
	API_MN Allocator
	allocator_arena_new(size_t block_size = 4096,
						Allocator meta_allocator = allocator_top());

	/**
	 * @brief      Ensures that the arena has the capacity for the given size
	 *
	 * @param[in]  arena      The arena
	 * @param[in]  grow_size  The grow size
	 */
	API_MN void
	allocator_arena_grow(Allocator arena, size_t grow_size);

	/**
	 * @brief      Frees all the buckets of an arena allocator
	 *
	 * @param[in]  arena  The arena
	 */
	API_MN void
	allocator_arena_free_all(Allocator arena);

	/**
	 * @brief      Returns the max used size(in bytes) through out the usage of this arena
	 *
	 * @param[in]  arena  The arena
	 */
	API_MN size_t
	allocator_arena_highwater(Allocator arena);

	/**
	 * @brief      Returns the current used memory size(in bytes) of the given arena
	 *
	 * @param[in]  arena  The arena
	 */
	API_MN size_t
	allocator_arena_used_size(Allocator arena);

	/**
	 * @brief      Creates a custom allocator
	 *
	 * @param      self   The object data which will be passed to the alloc and free functions
	 * @param[in]  alloc  The allocate function pointer
	 * @param[in]  free   The free function pointer
	 *
	 * @return     A Handle to the newly created allocator
	 */
	API_MN Allocator
	allocator_custom_new(void* self, Alloc_Func alloc, Free_Func free);

	/**
	 * @brief      Frees the allocator
	 *
	 * @param[in]  allocator  The allocator
	 */
	API_MN void
	allocator_free(Allocator allocator);


	//alloc from interface
	/**
	 * @brief      Allocates a block of memory off the given allocator
	 *
	 * @param[in]  allocator  The allocator
	 * @param[in]  size       The size
	 * @param[in]  alignment  The alignment
	 *
	 * @return     The allocated block of memory
	 */
	API_MN Block
	alloc_from(Allocator allocator, size_t size, uint8_t alignment);

	/**
	 * @brief      Frees a block of memory off the given allocator
	 *
	 * @param[in]  allocator  The allocator
	 * @param[in]  block      The block
	 */
	API_MN void
	free_from(Allocator allocator, Block block);


	//sugar coating
	/**
	 * @brief      Allocates a block of memory off the top of the allocator stack
	 *
	 * @param[in]  size       The size
	 * @param[in]  alignment  The alignment
	 */
	inline static Block
	alloc(size_t size, uint8_t alignment)
	{
		return alloc_from(allocator_top(), size, alignment);
	}

	/**
	 * @brief      Frees a block of memory off the top of the allocate stack
	 *
	 * @param[in]  block  The block
	 */
	inline static void
	free(Block block)
	{
		return free_from(allocator_top(), block);
	}

	/**
	 * @brief      Allocates a single instance of type T off the top of the allocator stack
	 *
	 * @tparam     T     Instance type
	 *
	 * @return     A Pointer to the allocated instance
	 */
	template<typename T>
	inline static T*
	alloc()
	{
		return (T*)alloc(sizeof(T), alignof(T)).ptr;
	}

	/**
	 * @brief      Allocates a single zero-initialized instance off the the top of the allocator stack
	 *
	 * @tparam     T     Instance type
	 *
	 * @return     A Pointer to the allocated instance
	 */
	template<typename T>
	inline static T*
	alloc_zerod()
	{
		Block block = alloc(sizeof(T), alignof(T));
		block_zero(block);
		return (T*)block.ptr;
	}

	/**
	 * @brief      Allocates a single instance of type T from the given allocator
	 *
	 * @param[in]  allocator  The allocator
	 *
	 * @tparam     T          Instance type
	 *
	 * @return     A Pointer to the allocated instance
	 */
	template<typename T>
	inline static T*
	alloc_from(Allocator allocator)
	{
		return (T*)alloc_from(allocator, sizeof(T), alignof(T)).ptr;
	}

	/**
	 * @brief      Allocates a single zero-initialized instance from the given allocator
	 *
	 * @param[in]  allocator  The allocator
	 *
	 * @tparam     T          Instance type
	 *
	 * @return     A Pointer to the allocated instance
	 */
	template<typename T>
	inline static T*
	alloc_zerod_from(Allocator allocator)
	{
		Block block = alloc_from(allocator, sizeof(T), alignof(T));
		block_zero(block);
		return (T*)block.ptr;
	}

	/**
	 * @brief      Frees a single instance off the top of the allocator stack
	 *
	 * @param[in]  ptr   The pointer
	 *
	 * @tparam     T     Instance type
	 */
	template<typename T>
	inline static void
	free(const T* ptr)
	{
		return free(Block { (void*)ptr, sizeof(T) });
	}

	/**
	 * @brief      Frees a single instance off the given allocator
	 *
	 * @param[in]  allocator  The allocator
	 * @param[in]  ptr        The pointer
	 *
	 * @tparam     T          Instance type
	 */
	template<typename T>
	inline static void
	free_from(Allocator allocator, const T* ptr)
	{
		return free_from(allocator, Block { (void*)ptr, sizeof(T) });
	}
}