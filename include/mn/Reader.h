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
	MS_HANDLE(Reader);
	MS_FWD_HANDLE(Stream);

	/**
	 * @brief      Returns the reader of the standard input
	 */
	API_MN Reader
	reader_stdin();

	API_MN Reader
	_reader_tmp();

	/**
	 * @brief      Returns a newly created reader on top of the given stream
	 *
	 * @param[in]  stream  The stream
	 */
	API_MN Reader
	reader_new(Stream stream);

	/**
	 * @brief      Returns a newly created reader on top of the given string (copies the string)
	 *
	 * @param[in]  str   The string
	 */
	API_MN Reader
	reader_str(const Str& str);

	/**
	 * @brief      Returns the given `reader` after configuring it to wrap the string string
	 *
	 * @param[in]  reader  The reader to edit (this is the returned value after it was edited)
	 * @param[in]  str     The string to wrap
	 */
	API_MN Reader
	reader_wrap_str(Reader reader, const Str& str);

	/**
	 * @brief      Frees the given reader
	 */
	API_MN void
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
	API_MN Block
	reader_peek(Reader reader, size_t size);

	/**
	 * @brief      Tries to skip over the underlying stream with the given size(in bytes)
	 *
	 * @param[in]  reader  The reader
	 * @param[in]  size    The size(in bytes)
	 *
	 * @return     The skipped size in bytes
	 */
	API_MN size_t
	reader_skip(Reader reader, size_t size);

	/**
	 * @brief      Tries to read from the reader into the given memory block
	 *
	 * @param[in]  reader  The reader
	 * @param[in]  data    The data
	 *
	 * @return     The read size in bytes
	 */
	API_MN size_t
	reader_read(Reader reader, Block data);
}
