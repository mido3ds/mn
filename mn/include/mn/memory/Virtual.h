#pragma once

#include "mn/Exports.h"
#include "mn/memory/Interface.h"
#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn::memory
{
	struct Virtual : Interface
	{
		~Virtual() = default;

		MN_EXPORT Block
		alloc(size_t size, uint8_t alignment) override;

		MN_EXPORT void
		free(Block block) override;
	};

	MN_EXPORT Virtual*
	virtual_mem();
}
