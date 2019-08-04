#include "mn/io/Stream.h"

#include <mn/Memory.h>

namespace mn::io
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

	void
	stream_free(Stream self)
	{
		free_destruct(self);
	}
}