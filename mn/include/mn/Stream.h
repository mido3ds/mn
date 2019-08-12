#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

#include <stdint.h>

namespace mn
{
	typedef struct IStream* Stream;
	struct IStream
	{
		virtual ~IStream() = default;
		virtual size_t read(Block data) = 0;
		virtual size_t write(Block data) = 0;
		virtual int64_t size() = 0;
	};

	MN_EXPORT size_t
	stream_read(Stream self, Block data);

	MN_EXPORT size_t
	stream_write(Stream self, Block data);

	//if the stream has no size (like socket, etc..) -1 is returned
	MN_EXPORT int64_t
	stream_size(Stream self);

	MN_EXPORT void
	stream_free(Stream self);

	inline static void
	destruct(Stream self)
	{
		stream_free(self);
	}
}