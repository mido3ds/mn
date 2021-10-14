#pragma once

#include "mn/Exports.h"
#include "mn/Assert.h"

#include <stddef.h>
#include <stdint.h>

namespace mn
{
	// represents a block of memory
	struct Block
	{
		// pointer to the memory block
		void* ptr;

		// size of the memory block in bytes
		size_t size;
	};

	// returns whether the given memory block is empty (points to null or size is 0 bytes)
	inline static bool
	block_is_empty(Block self)
	{
		return self.ptr == nullptr || self.size == 0;
	}

	// sets the entire block of memory to 0 (similar to memset to 0)
	MN_EXPORT void
	block_zero(Block block);

	// wraps a C null-terminated string in a memory block (does not include the null termination)
	MN_EXPORT Block
	block_lit(const char* str);

	// wraps any value T into a memory block by taking it's address and size
	template<typename T>
	inline static Block
	block_from(const T& value)
	{
		return Block {(void*)&value, sizeof(T)};
	}

	// wraps a pointer to type T into a memory block of the same address as the pointer and same size as sizeof(T)
	template<typename T>
	inline static Block
	block_from_ptr(const T* value)
	{
		return Block {(void*)value, sizeof(T)};
	}

	// advances the block pointer and decreases the size, think of offset as a point which divides the block in two
	// and we always take the right part
	inline static Block
	operator+(const Block& block, size_t offset)
	{
		mn_assert(block.size >= offset);
		return Block { (char*)block.ptr + offset, block.size - offset };
	}

	// advances the block pointer and decreases the size, think of offset as a point which divides the block in two
	// and we always take the right part
	inline static Block
	operator+(size_t offset, const Block& block)
	{
		mn_assert(block.size >= offset);
		return Block { (char*)block.ptr + offset, block.size - offset };
	}

	// moves the block pointer backwards and increases the size, think of offset as a point that lays at the left of the
	// block and we then return the new block which starts at the offset point and continus to the end of the block
	// which is the opposite of the above operator+
	inline static Block
	operator-(const Block& block, size_t offset)
	{
		return Block { (char*)block.ptr - offset, block.size + offset };
	}

	// moves the block pointer backwards and increases the size, think of offset as a point that lays at the left of the
	// block and we then return the new block which starts at the offset point and continus to the end of the block
	// which is the opposite of the above operator+
	inline static Block
	operator-(size_t offset, const Block& block)
	{
		return Block { (char*)block.ptr - offset, block.size + offset };
	}

	// defines timeout in milliseconds, which is used in various async operations
	struct Timeout
	{
		uint64_t milliseconds;

		bool operator==(Timeout other) const { return milliseconds == other.milliseconds; }
		bool operator!=(Timeout other) const { return milliseconds != other.milliseconds; }
	};

	// constant which represents no timeout
	constexpr inline Timeout NO_TIMEOUT{ 0 };
	// constant which represents an infinite timeout
	constexpr inline Timeout INFINITE_TIMEOUT{ 0xFFFFFFFFFFFFFFFF };

	// a tracy compatible source location struct that's used with mutexes
	struct Source_Location
	{
		const char* name;
		const char* function;
		const char* file;
		uint32_t line;
		uint32_t color;
	};
}
