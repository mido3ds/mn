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
	atomic_dec(int32_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_sub_fetch(&self, 1, __ATOMIC_SEQ_CST);
	}

	int64_t
	atomic_dec(int64_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return __atomic_sub_fetch(&self, 1, __ATOMIC_SEQ_CST);
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

	int32_t
	atomic_exchange(int32_t &self, int32_t value)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		int32_t res = 0;
		__atomic_exchange(&self, &value, &res, __ATOMIC_SEQ_CST);
		return res;
	}

	int64_t
	atomic_exchange(int64_t &self, int64_t value)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		int64_t res = 0;
		__atomic_exchange(&self, &value, &res, __ATOMIC_SEQ_CST);
		return res;
	}

	int32_t
	atomic_load(int32_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		int32_t res = 0;
		__atomic_load(&self, &res, __ATOMIC_SEQ_CST);
		return res;
	}

	int64_t
	atomic_load(int64_t &self)
	{
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		int64_t res = 0;
		__atomic_load(&self, &res, __ATOMIC_SEQ_CST);
		return res;
	}
}
