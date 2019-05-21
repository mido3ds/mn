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

		MN_EXPORT
		Stack(size_t stack_size, Interface* meta = clib());

		MN_EXPORT
		~Stack() override;

		MN_EXPORT Block
		alloc(size_t size, uint8_t alignment) override;

		MN_EXPORT void
		free(Block block) override;

		MN_EXPORT void
		free_all();
	};
}
