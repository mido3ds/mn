#include "mn/Atomic.h"

#include <assert.h>

namespace mn
{
	int32_t
	atomic_inc(int32_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_add_fetch(&self, 1, __ATOMIC_SEQ_CST);
	}

	int64_t
	atomic_inc(int64_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_add_fetch(&self, 1, __ATOMIC_SEQ_CST);
	}

	int32_t
	atomic_add(int32_t &self, int32_t value)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_fetch_add(&self, value, __ATOMIC_SEQ_CST);
	}

	int64_t
	atomic_add(int64_t &self, int64_t value)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_fetch_add(&self, value, __ATOMIC_SEQ_CST);
	}
}
