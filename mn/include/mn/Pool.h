#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Thread.h"
#include "mn/Memory.h"

namespace mn
{
	// memory pool handle
	typedef struct IPool* Pool;

	// creates a new memory pool for the given element size, internally the pool uses buckets of the
	// given bucket_size of elements and using the meta allocator to allocate more memory
	MN_EXPORT Pool
	pool_new(size_t element_size, size_t bucket_size, Allocator meta_allocator = allocator_top());

	// frees the given memory pool
	MN_EXPORT void
	pool_free(Pool pool);

	// destruct overload for pool free
	inline static void
	destruct(Pool pool)
	{
		pool_free(pool);
	}

	// returns a memory suitable to write an object of size element_size used in creation function
	MN_EXPORT void*
	pool_get(Pool pool);

	// puts back the given memory into the pool to be reused later
	MN_EXPORT void
	pool_put(Pool pool, void* ptr);
}