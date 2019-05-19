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

		API_MN
		Arena(size_t block_size, Interface* meta = clib());

		API_MN
		~Arena() override;

		API_MN Block
		alloc(size_t size, uint8_t alignment) override;

		API_MN void
		free(Block block) override;

		API_MN void
		grow(size_t size);

		API_MN void
		free_all();
	};

	API_MN Arena*
	tmp();
}
