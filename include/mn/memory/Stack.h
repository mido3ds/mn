#pragma once

#include "mn/Exports.h"
#include "mn/memory/Interface.h"
#include "mn/memory/CLib.h"
#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn::memory
{
	struct Stack : Interface
	{
		Interface* meta;
		Block memory;
		uint8_t* alloc_head;
		size_t allocations_count;

		API_MN
		Stack(size_t stack_size, Interface* meta = clib());

		API_MN
		~Stack() override;

		API_MN Block
		alloc(size_t size, uint8_t alignment) override;

		API_MN void
		free(Block block) override;

		API_MN void
		free_all();
	};
}
