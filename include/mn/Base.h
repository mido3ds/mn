#pragma once

#include "mn/Exports.h"

#include <stddef.h>

/**
 * @brief      Declares a handle which is a pointer to a generated empty structure
 * 			   `typedef struct NAME##__ { int unused; } *NAME;`
 *
 * @param      NAME  The Name of the handle
 */
#define MS_HANDLE(NAME) typedef struct NAME##__ { int unused; } *NAME;

/**
 * @brief      Forward declares a handle by name `typedef struct NAME##__ *NAME;`
 *
 * @param      NAME  The Name of the handle
 */
#define MS_FWD_HANDLE(NAME) typedef struct NAME##__ *NAME;

namespace mn
{
	/**
	 * @brief      A Representation of a block of memory
	 */
	struct Block
	{
		//pointer to the memory block
		void*  ptr;

		//size of the memory block in bytes
		size_t size;

		/**
		 * @brief      Implicit casting operator to boolean for traditional `if(block)` testing
		 */
		operator bool() const
		{
			return ptr != nullptr && size != 0;
		}
	};

	/**
	 * @brief      Sets all the bytes in the block to 0
	 *
	 * @param[in]  block  The block to set
	 */
	API_MN void
	block_zero(Block block);

	/**
	 * @brief      Wraps a C string literal into a block
	 *
	 * @param[in]  str   The C string
	 *
	 * @return     A Block enclosing the string without the null termination
	 */
	API_MN Block
	block_lit(const char* str);

	/**
	 * @brief      Wraps any value T into a block
	 *
	 * @param[in]  value  The Value to be wraped
	 *
	 * @tparam     T          The Type of the value
	 *
	 * @return     A Block enclosing the value
	 */
	template<typename T>
	inline static Block
	block_from(const T& value)
	{
		return Block {(void*)&value, sizeof(T)};
	}

	/**
	 * @brief      Wraps a pointer type to a block (assumes pointer
	 * 			   is pointing to a single instance of type T)
	 *
	 * @param[in]  value  The Pointer to the value to be wraped
	 *                    
	 * @tparam     T      The Type of the pointer
	 *
	 * @return     A Block enclosing the object which the pointer points to (uses `sizeof(T)`)
	 */
	template<typename T>
	inline static Block
	block_from_ptr(const T* value)
	{
		return Block {(void*)value, sizeof(T)};
	}

	//offseting the block
	/**
	 * @brief      Advances the block pointer and decreases the size, think of the offset
	 * 			   as a point which divides the block in two and we always take the right half
	 *
	 * @param[in]  block   The block
	 * @param[in]  offset  The offset
	 *
	 * @return     The New shrunk block
	 */
	inline static Block
	operator+(const Block& block, size_t offset)
	{
		return Block { (char*)block.ptr + offset, block.size - offset };
	}

	/**
	 * @brief      Advances the block pointer and decreases the size, think of the offset
	 * 			   as a point which divides the block in two and we always take the right half
	 *
	 * @param[in]  offset  The offset
	 * @param[in]  block   The block
	 *
	 * @return     The New shrunk block
	 */
	inline static Block
	operator+(size_t offset, const Block& block)
	{
		return Block { (char*)block.ptr + offset, block.size - offset };
	}

	/**
	 * @brief      Moves the block pointer backwards and increases the size, think of
	 * 			   the offset as a point that lays at the left of the block and we then
	 * 			   return the new block which starts at the offset point and encloses till
	 * 			   the end of the block
	 *	
	 * @param[in]  block   The block
	 * @param[in]  offset  The offset
	 *
	 * @return     The New enlarged block
	 */
	inline static Block
	operator-(const Block& block, size_t offset)
	{
		return Block { (char*)block.ptr - offset, block.size + offset };
	}

	/**
	 * @brief      Moves the block pointer backwards and increases the size, think of
	 * 			   the offset as a point that lays at the left of the block and we then
	 * 			   return the new block which starts at the offset point and encloses till
	 * 			   the end of the block
	 *
	 * @param[in]  offset  The offset
	 * @param[in]  block   The block
	 *
	 * @return     The New enlarged block
	 */
	inline static Block
	operator-(size_t offset, const Block& block)
	{
		return Block { (char*)block.ptr - offset, block.size + offset };
	}
}
