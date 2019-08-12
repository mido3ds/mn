#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/Str.h"

namespace mn
{
	typedef struct IMemory_Stream* Memory_Stream;
	struct IMemory_Stream final: IStream
	{
		mn::Str str;
		int64_t cursor;

		MN_EXPORT virtual
		~IMemory_Stream() override;

		MN_EXPORT virtual size_t
		read(Block data) override;

		MN_EXPORT virtual size_t
		write(Block data) override;

		MN_EXPORT virtual int64_t
		size() override;
	};

	/**
	 * @brief      Creates new memory stream
	 *
	 * @param[in]  allocator  The allocator to be used (optional by default will take
	 * whatever on top of the allocator stack)
	 */
	MN_EXPORT Memory_Stream
	memory_stream_new(Allocator allocator = allocator_top());

	/**
	 * @brief      Frees the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT void
	memory_stream_free(Memory_Stream self);

	/**
	 * @brief      Destruct function overload for the memory stream
	 *
	 * @param      self  The memory stream
	 */
	inline static void
	destruct(Memory_Stream self)
	{
		memory_stream_free(self);
	}

	/**
	 * @brief      Writes the given block in the memory stream
	 *
	 * @param      self  The memory stream
	 * @param[in]  data  The data block
	 *
	 * @return     The size of the written data in bytes
	 */
	MN_EXPORT size_t
	memory_stream_write(Memory_Stream self, Block data);

	/**
	 * @brief      Reads into the given block from the memory stream
	 *
	 * @param      self  The memory stream
	 * @param[in]  data  The data block
	 *
	 * @return     The size of the read data in bytes
	 */
	MN_EXPORT size_t
	memory_stream_read(Memory_Stream self, Block data);

	/**
	 * @brief      Returns the size of the memory stream in bytes
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT int64_t
	memory_stream_size(Memory_Stream self);

	MN_EXPORT bool
	memory_stream_eof(Memory_Stream self);

	/**
	 * @brief      Returns the position of the cursor in the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT int64_t
	memory_stream_cursor_pos(Memory_Stream self);

	/**
	 * @brief      Moves the cursor by the given offset
	 *
	 * @param      self    The memory stream
	 * @param[in]  offset  The offset
	 */
	MN_EXPORT void
	memory_stream_cursor_move(Memory_Stream self, int64_t offset);

	/**
	 * @brief      Moves the cursor to the given absolute position
	 *
	 * @param      self  The memory stream
	 * @param[in]  abs   The absolute position
	 */
	MN_EXPORT void
	memory_stream_cursor_set(Memory_Stream self, int64_t abs);

	/**
	 * @brief      Moves the cursor to the start of the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT void
	memory_stream_cursor_to_start(Memory_Stream self);

	/**
	 * @brief      Moves the cursor to the end of the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT void
	memory_stream_cursor_to_end(Memory_Stream self);

	/**
	 * @brief      Ensures the memory stream has the capacity to hold the given size(in bytes)
	 *
	 * @param      self  The memory stream
	 * @param[in]  size  The size (in bytes)
	 */
	MN_EXPORT void
	memory_stream_reserve(Memory_Stream self, size_t size);

	/**
	 * @brief      Returns the capacity of the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT size_t
	memory_stream_capacity(Memory_Stream self);

	/**
	 * @brief      Clears the memory stream
	 *
	 * @param      self  The memory stream
	 */
	MN_EXPORT void
	memory_stream_clear(Memory_Stream self);

	/**
	 * @brief      Returns the memory block starting from the cursor with the given
	 * size(in bytes) advancing towards the end of the stream
	 * memory_stream_block_ahead([abcd|efghe], 2) -> [ef]
	 *
	 * @param      self  The memory stream
	 * @param[in]  size  The size (in bytes)
	 */
	MN_EXPORT Block
	memory_stream_block_ahead(Memory_Stream self, size_t size);

	/**
	 * @brief      Returns the memory block starting from the cursor with the given
	 * size(in bytes) advancing towards the start of the stream
	 * memory_stream_block_behind([abcd|efghe], 2) -> [cd]
	 *
	 * @param      self  The memory stream
	 * @param[in]  size  The size (in bytes)
	 */
	MN_EXPORT Block
	memory_stream_block_behind(Memory_Stream self, size_t size);

	/**
	 * @brief      Pipes data into the memory stream with the given size
	 *
	 * @param      self    The memory stream to pipe the data into
	 * @param[in]  stream  The stream to pipe the data from
	 * @param[in]  size    The size(in bytes) of the data to be piped
	 *
	 * @return     The size of the piped data(in bytes)
	 */
	MN_EXPORT size_t
	memory_stream_pipe(Memory_Stream self, Stream stream, size_t size);

	inline static const char*
	memory_stream_ptr(Memory_Stream self)
	{
		return self->str.ptr;
	}

	inline static Str
	memory_stream_str(Memory_Stream self)
	{
		Str res = self->str;
		self->str = str_with_allocator(self->str.allocator);
		self->cursor = 0;
		return res;
	}
}