#pragma once

#include "mn/Exports.h"
#include "mn/memory/Interface.h"
#include "mn/Base.h"
#include "mn/Str.h"
#include "mn/Thread.h"
#include "mn/Atomic.h"

#include <stdint.h>

namespace mn::memory
{
	struct Fast_Leak: Interface
	{
		int64_t atomic_size;
		int64_t atomic_count;

		MN_EXPORT
		Fast_Leak();

		MN_EXPORT
		~Fast_Leak() override;

		MN_EXPORT Block
		alloc(size_t size, uint8_t alignment) override;

		MN_EXPORT void
		free(Block block) override;
	};

	MN_EXPORT Fast_Leak*
	fast_leak();
}
