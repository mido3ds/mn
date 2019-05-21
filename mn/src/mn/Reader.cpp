#include "mn/Reader.h"
#include "mn/Stream.h"
#include "mn/Memory_Stream.h"
#include "mn/Pool.h"

namespace mn
{
	struct IReader
	{
		Stream stream;
		Memory_Stream buffer;
	};
	
	TS_Typed_Pool<IReader>*
	_reader_pool()
	{
		static TS_Typed_Pool<IReader> _pool(1024, memory::clib());
		return &_pool;
	}

	IReader
	_reader_stdin()
	{
		IReader self{};
		self.stream = stream_stdin();
		self.buffer = memory_stream_new();
		return self;
	}

	Reader
	reader_stdin()
	{
		static IReader _stdin = _reader_stdin();
		return &_stdin;
	}

	struct Reader_Tmp_Wrapper
	{
		IReader self;

		Reader_Tmp_Wrapper()
		{
			self.stream = nullptr;
			self.buffer = memory_stream_new(memory::clib());
		}

		~Reader_Tmp_Wrapper()
		{
			memory_stream_free(self.buffer);
		}
	};

	Reader
	_reader_tmp()
	{
		thread_local Reader_Tmp_Wrapper _reader;
		return &_reader.self;
	}

	Reader
	reader_new(Stream stream)
	{
		Reader self = _reader_pool()->get();
		self->stream = stream;
		self->buffer = memory_stream_new();
		return self;
	}

	Reader
	reader_str(const Str& str)
	{
		Reader self = _reader_pool()->get();
		self->stream = nullptr;
		self->buffer = memory_stream_new();
		memory_stream_write(self->buffer, Block{ str.ptr, str.count });
		memory_stream_cursor_to_start(self->buffer);
		return self;
	}

	Reader
	reader_wrap_str(Reader self, const Str& str)
	{
		if(self == nullptr)
			return reader_str(str);

		assert(self->stream == nullptr);
		memory_stream_clear(self->buffer);
		memory_stream_write(self->buffer, Block { str.ptr, str.count });
		memory_stream_cursor_to_start(self->buffer);
		return self;
	}

	void
	reader_free(Reader self)
	{
		memory_stream_free(self->buffer);
		if(self->stream)
			stream_free(self->stream);

		_reader_pool()->put(self);
	}

	Block
	reader_peek(Reader self, size_t size)
	{
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
	reader_skip(Reader self, size_t size)
	{
		//get the available data in the buffer
		size_t available_size = self->buffer.str.count - self->buffer.cursor;

		size_t result = available_size < size ? available_size : size;
		memory_stream_cursor_move(self->buffer, result);
		if(self->buffer.str.count - self->buffer.cursor == 0)
			memory_stream_clear(self->buffer);
		return result;
	}

	size_t
	reader_read(Reader self, Block data)
	{
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
