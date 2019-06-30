#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Str.h"

namespace mn
{
	/**
	 * Reader Handle
	 * A Reader is a form of buffered stream which is suitable for parsing
	 * text/string input like the usage in function `reads`
	 */
	typedef struct IReader* Reader;
	typedef struct IStream* Stream;

	/**
	 * @brief      Returns the reader of the standard input
	 */
	MN_EXPORT Reader
	reader_stdin();

	/**
	 * @brief      Returns a newly created reader on top of the given stream
	 *
	 * @param[in]  stream  The stream
	 */
	MN_EXPORT Reader
	reader_new(Stream stream);

	MN_EXPORT Reader
	reader_with_allocator(Stream stream, Allocator allocator);

	/**
	 * @brief      Returns a newly created reader on top of the given string (copies the string)
	 *
	 * @param[in]  str   The string
	 */
	MN_EXPORT Reader
	reader_str(const Str& str);

	/**
	 * @brief      Returns the given `reader` after configuring it to wrap the string string
	 *
	 * @param[in]  reader  The reader to edit (this is the returned value after it was edited)
	 * @param[in]  str     The string to wrap
	 */
	MN_EXPORT Reader
	reader_wrap_str(Reader reader, const Str& str);

	/**
	 * @brief      Frees the given reader
	 */
	MN_EXPORT void
	reader_free(Reader reader);

	/**
	 * @brief      Destruct function overload for the reader type
	 */
	inline static void
	destruct(Reader reader)
	{
		reader_free(reader);
	}

	/**
	 * @brief      Tries to peek into the underlying stream with the given size(in bytes)
	 *
	 * @param[in]  reader  The reader
	 * @param[in]  size    The size(in bytes)
	 *
	 * @return     Memory Block of the peeked content
	 */
	MN_EXPORT Block
	reader_peek(Reader reader, size_t size);

	/**
	 * @brief      Tries to skip over the underlying stream with the given size(in bytes)
	 *
	 * @param[in]  reader  The reader
	 * @param[in]  size    The size(in bytes)
	 *
	 * @return     The skipped size in bytes
	 */
	MN_EXPORT size_t
	reader_skip(Reader reader, size_t size);

	/**
	 * @brief      Tries to read from the reader into the given memory block
	 *
	 * @param[in]  reader  The reader
	 * @param[in]  data    The data
	 *
	 * @return     The read size in bytes
	 */
	MN_EXPORT size_t
	reader_read(Reader reader, Block data);
}
