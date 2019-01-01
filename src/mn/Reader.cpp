#include "mn/Reader.h"
#include "mn/Stream.h"
#include "mn/Memory_Stream.h"
#include "mn/Pool.h"

namespace mn
{
	struct Internal_Reader
	{
		Stream stream;
		Memory_Stream buffer;
	};
	
	TS_Typed_Pool<Internal_Reader>*
	_reader_pool()
	{
		static TS_Typed_Pool<Internal_Reader> _pool(1024, clib_allocator);
		return &_pool;
	}

	Internal_Reader
	_reader_stdin()
	{
		Internal_Reader self{};
		self.stream = stream_stdin();
		self.buffer = memory_stream_new();
		return self;
	}

	Reader
	reader_stdin()
	{
		static Internal_Reader _stdin = _reader_stdin();
		return (Reader)&_stdin;
	}

	Reader
	reader_new(Stream stream)
	{
		Internal_Reader* self = _reader_pool()->get();
		self->stream = stream;
		self->buffer = memory_stream_new();
		return (Reader)self;
	}

	Reader
	reader_str(const Str& str)
	{
		Internal_Reader* self = _reader_pool()->get();
		self->stream = nullptr;
		self->buffer = memory_stream_new();
		memory_stream_write(self->buffer, Block{ str.ptr, str.count });
		memory_stream_cursor_to_start(self->buffer);
		return (Reader)self;
	}

	Reader
	reader_wrap_str(Reader reader, const Str& str)
	{
		if(reader == nullptr)
			return reader_str(str);

		Internal_Reader* self = (Internal_Reader*)reader;
		assert(self->stream == nullptr);
		memory_stream_clear(self->buffer);
		memory_stream_write(self->buffer, Block { str.ptr, str.count });
		memory_stream_cursor_to_start(self->buffer);
		return reader;
	}

	void
	reader_free(Reader reader)
	{
		Internal_Reader* self = (Internal_Reader*)reader;

		memory_stream_free(self->buffer);
		if(self->stream)
			stream_free(self->stream);

		_reader_pool()->put(self);
	}

	Block
	reader_peek(Reader reader, size_t size)
	{
		Internal_Reader* self = (Internal_Reader*)reader;

		//get the available data in the buffer
		size_t available_size = self->buffer.str.count - self->buffer.cursor;

		//if the user needs to peek on the already buffered data the return it as is
		if(size == 0)
			return memory_stream_block_ahead(self->buffer, available_size);

		//save the old cursor
		int64_t old_cursor = self->buffer.cursor;
		if(available_size < size)
		{
			size_t diff = size - available_size;
			memory_stream_cursor_to_end(self->buffer);
			if(self->stream)
				available_size += memory_stream_pipe(self->buffer, self->stream, diff);
			self->buffer.cursor = old_cursor;
		}
		return memory_stream_block_ahead(self->buffer, available_size);
	}

	size_t
	reader_skip(Reader reader, size_t size)
	{
		Internal_Reader* self = (Internal_Reader*)reader;

		//get the available data in the buffer
		size_t available_size = self->buffer.str.count - self->buffer.cursor;

		size_t result = available_size < size ? available_size : size;
		memory_stream_cursor_move(self->buffer, result);
		if(self->buffer.str.count - self->buffer.cursor == 0)
			memory_stream_clear(self->buffer);
		return result;
	}

	size_t
	reader_read(Reader reader, Block data)
	{
		Internal_Reader* self = (Internal_Reader*)reader;

		//empty request
		if(data.size == 0)
			return 0;

		size_t request_size = data.size;
		size_t read_size = 0;
		//get the available data in the buffer
		size_t available_size = self->buffer.str.count - self->buffer.cursor;
		if(available_size > 0)
		{
			read_size += memory_stream_read(self->buffer, data);
			request_size -= read_size;
		}

		if(request_size == 0)
			return read_size;

		memory_stream_clear(self->buffer);
		if(self->stream)
			read_size += stream_read(self->stream, data + read_size);
		return read_size;
	}
}
