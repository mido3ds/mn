#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Str.h"
#include "mn/Stream.h"

namespace mn
{
	// a reader handle
	// a reader is a form of buffered stream which is suitable for parsing
	// text/string input like the usage in function `reads`
	typedef struct IReader* Reader;

	// returns the read of the standard input stream
	MN_EXPORT Reader
	reader_stdin();

	// returns a newly created reader on top of the given stream
	MN_EXPORT Reader
	reader_new(Stream stream, Allocator allocator = allocator_top());

	// returns a newly created reader on top of the given string (copies the string internally)
	MN_EXPORT Reader
	reader_str(const Str& str);

	// returns the given reader after configuring it to wrapt the string
	MN_EXPORT Reader
	reader_wrap_str(Reader reader, const Str& str);

	// returns the given reader after configuring it to wrapt the string
	inline static Reader
	reader_wrap_str(Reader reader, const char* str)
	{
		return reader_wrap_str(reader, str_lit(str));
	}

	// frees the given reader
	MN_EXPORT void
	reader_free(Reader reader);

	// destruct overload for reader free
	inline static void
	destruct(Reader reader)
	{
		reader_free(reader);
	}

	// tries to peek into the underlying stream with the given size (in bytes)
	MN_EXPORT Block
	reader_peek(Reader reader, size_t size);

	// tries to skip over the underlying stream with the given size (in bytes)
	MN_EXPORT size_t
	reader_skip(Reader reader, size_t size);

	// tries to read from the reader into the given memory block
	MN_EXPORT size_t
	reader_read(Reader reader, Block data);

	// returns the size of the consumed in bytes
	MN_EXPORT size_t
	reader_consumed(Reader reader);

	// returns reader progress if the underlying stream supports size operation
	// if you have read half of the file it will return 0.5
	// if stream doesn't support size, like sockets, etc.. it will return 0
	MN_EXPORT float
	reader_progress(Reader reader);
}
