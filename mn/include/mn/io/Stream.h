#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

namespace mn::io
{
	typedef struct IStream* Stream;
	struct IStream
	{
		virtual ~IStream() = default;
		virtual size_t read(Block data) = 0;
		virtual size_t write(Block data) = 0;
	};

	MN_EXPORT size_t
	stream_read(Stream self, Block data);

	MN_EXPORT size_t
	stream_write(Stream self, Block data);

	MN_EXPORT void
	stream_free(Stream self);

	inline static void
	destruct(Stream self)
	{
		stream_free(self);
	}
}