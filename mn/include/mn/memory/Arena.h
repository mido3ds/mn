#pragma once

#include "mn/Exports.h"
#include "mn/memory/Interface.h"
#include "mn/memory/CLib.h"
#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn::memory
{
	struct Arena : Interface
	{
		struct Node
		{
			Block mem;
			uint8_t* alloc_head;
			Node* next;
		};

		Interface* meta;
		Node* root;
		size_t block_size, total_mem, used_mem, highwater_mem;
		// determines the threshold amount of temporary memory between the current and previous highwater
		// that will trigger a readjust (free/realloc), default value is 4MB
		size_t clear_all_readjust_threshold;
		size_t clear_all_current_highwater;
		size_t clear_all_previous_highwater;

		MN_EXPORT
		Arena(size_t block_size, Interface* meta = clib());

		MN_EXPORT
		~Arena() override;

		MN_EXPORT Block
		alloc(size_t size, uint8_t alignment) override;

		MN_EXPORT void
		free(Block block) override;

		MN_EXPORT void
		grow(size_t size);

		MN_EXPORT void
		free_all();

		MN_EXPORT void
		clear_all();

		MN_EXPORT bool
		owns(void* ptr);
	};
}
