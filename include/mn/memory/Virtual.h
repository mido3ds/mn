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

		API_MN Block
		alloc(size_t size, uint8_t alignment) override;

		API_MN void
		free(Block block) override;
	};

	API_MN Virtual*
	virtual_mem();
}
