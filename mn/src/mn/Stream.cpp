#include "mn/Stream.h"

#include <mn/Memory.h>

namespace mn
{
	//API
	size_t
	stream_read(Stream self, Block data)
	{
		return self->read(data);
	}

	size_t
	stream_write(Stream self, Block data)
	{
		return self->write(data);
	}

	int64_t
	stream_size(Stream self)
	{
		return self->size();
	}

	void
	stream_free(Stream self)
	{
		self->dispose();
	}
}