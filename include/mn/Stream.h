#pragma once

#include "mn/Exports.h"
#include "mn/File.h"
#include "mn/Base.h"
#include "mn/Memory_Stream.h"

namespace mn
{
	/**
	 * Stream handle
	 */
	MS_HANDLE(Stream);

	/**
	 * @brief      Returns a stream of the standard output
	 */
	API_MN Stream
	stream_stdout();

	/**
	 * @brief      Returns a stream of the standard error
	 */
	API_MN Stream
	stream_stderr();

	/**
	 * @brief      Returns a stream of the standard input
	 */
	API_MN Stream
	stream_stdin();

	/**
	 * @brief      Returns a stream of the given file
	 *
	 * @param[in]  filename   The filename
	 * @param[in]  io_mode    The i/o mode
	 * @param[in]  open_mode  The open mode
	 */
	API_MN Stream
	stream_file_new(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode);

	/**
	 * @brief      Returns a stream over a `Memory_Stream`
	 *
	 * @param[in]  allocator  The allocator to be used by the memory stream
	 */
	API_MN Stream
	stream_memory_new(Allocator allocator = allocator_top());

	/**
	 * @brief      Frees the given stream
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN void
	stream_free(Stream stream);

	/**
	 * @brief      Destruct function overload for the stream type
	 *
	 * @param[in]  stream  The stream
	 */
	inline static void
	destruct(Stream stream)
	{
		stream_free(stream);
	}

	/**
	 * @brief      Writes the given block into the stream
	 *
	 * @param[in]  stream  The stream
	 * @param[in]  data    The data
	 *
	 * @return     The Written size in bytes
	 */
	API_MN size_t
	stream_write(Stream stream, Block data);

	/**
	 * @brief      Reads from the stream into the given data
	 *
	 * @param[in]  stream  The stream
	 * @param[in]  data    The data
	 *
	 * @return     The Read size in bytes
	 */
	API_MN size_t
	stream_read(Stream stream, Block data);

	/**
	 * @brief      Returns the size of the stream(in bytes)
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN int64_t
	stream_size(Stream stream);

	/**
	 * @brief      Returns the cursor position of the stream
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN int64_t
	stream_cursor_pos(Stream stream);

	/**
	 * @brief      Moves the cursor of the stream with the given offset
	 *
	 * @param[in]  stream  The stream
	 * @param[in]  offset  The offset
	 */
	API_MN void
	stream_cursor_move(Stream stream, int64_t offset);

	/**
	 * @brief      Moves the cursor to the start of the stream
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN void
	stream_cursor_move_to_start(Stream stream);

	/**
	 * @brief      Moves the cursor to the end of the stream
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN void
	stream_cursor_move_to_end(Stream stream);

	/**
	 * @brief      Returns a string to the underlying data of the stream
	 * Works with the memory stream only
	 * 
	 * @param[in]  stream  The stream
	 */
	API_MN const char*
	stream_str(Stream stream);

	/**
	 * @brief      Pipes data into the memory stream with the given size
	 *
	 * @param      self    The memory stream to pipe the data into
	 * @param[in]  stream  The stream to pipe the data from
	 * @param[in]  size    The size(in bytes) of the data to be piped
	 *
	 * @return     The size of the piped data(in bytes)
	 */
	API_MN size_t
	memory_stream_pipe(Memory_Stream& self, Stream stream, size_t size);

	/**
	 * @brief      Pipes data into the memory stream with the given size
	 *
	 * @param      self    The memory stream to pipe the data into
	 * @param[in]  file    The file to pipe the data from
	 * @param[in]  size    The size(in bytes) of the data to be piped
	 *
	 * @return     The size of the piped data(in bytes)
	 */
	API_MN size_t
	memory_stream_pipe(Memory_Stream& self, File file, size_t size);
}
