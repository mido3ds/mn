#include "mn/Atomic.h"

#include <Windows.h>

#include <assert.h>

namespace mn
{
	int32_t
	atomic_inc(int32_t &self)
	{
		static_assert(sizeof(LONG) == sizeof(int32_t));
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return InterlockedIncrement((LONG*)&self);
	}

	int64_t
	atomic_inc(int64_t &self)
	{
		static_assert(sizeof(LONG64) == sizeof(int64_t));
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return InterlockedIncrement64((LONG64*)&self);
	}

	int32_t
	atomic_add(int32_t &self, int32_t value)
	{
		static_assert(sizeof(LONG) == sizeof(int32_t));
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return InterlockedExchangeAdd((LONG*)&self, LONG(value));
	}

	int64_t
	atomic_add(int64_t &self, int64_t value)
	{
		static_assert(sizeof(LONG64) == sizeof(int64_t));
		assert(uintptr_t(&self) % sizeof(self) == 0 && "atomic is not aligned");
		return InterlockedExchangeAdd64((LONG64*)&self, LONG64(value));
	}
}
