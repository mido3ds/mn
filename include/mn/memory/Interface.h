#pragma once

#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn::memory
{
	struct Interface
	{
		virtual ~Interface() = default;
		virtual Block alloc(size_t size, uint8_t alignment) = 0;
		virtual void free(Block block) = 0;
	};
}
