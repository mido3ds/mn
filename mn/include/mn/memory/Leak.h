#pragma once

#include "mn/Exports.h"
#include "mn/memory/Interface.h"
#include "mn/Base.h"
#include "mn/Str.h"
#include "mn/Thread.h"

#include <stdint.h>
#include <stddef.h>

namespace mn::memory
{
	struct Leak: Interface
	{
		struct Node
		{
			size_t size;
			Str callstack;
			Node* next;
			Node* prev;
		};

		Node* head;
		Mutex mtx;

		API_MN
		Leak();

		API_MN
		~Leak() override;

		API_MN Block
		alloc(size_t size, uint8_t alignment) override;

		API_MN void
		free(Block block) override;
	};

	API_MN Leak*
	leak();
}
