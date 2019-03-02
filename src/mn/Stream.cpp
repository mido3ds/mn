#include "mn/Stream.h"
#include "mn/Memory_Stream.h"
#include "mn/Pool.h"

//to supress warnings in release mode
#define UNUSED(x) ((void)(x))

namespace mn
{
	struct Internal_Stream
	{
		enum KIND
		{
			KIND_NONE,
			KIND_FILE,
			KIND_MEMORY
		};

		KIND kind;
		union
		{
			File file;
			Memory_Stream memory;
		};
	};
	TS_Typed_Pool<Internal_Stream>*
	_stream_pool()
	{
		static TS_Typed_Pool<Internal_Stream> _pool(1024, clib_allocator);
		return &_pool;
	}

	Internal_Stream
	_stream_std(File file)
	{
		Internal_Stream self{};
		self.kind = Internal_Stream::KIND_FILE;
		self.file = file;
		return self;
	}

	Stream
	stream_stdout()
	{
		static Internal_Stream _stdout = _stream_std(file_stdout());
		return (Stream)&_stdout;
	}

	Stream
	stream_stderr()
	{
		static Internal_Stream _stderr = _stream_std(file_stderr());
		return (Stream)&_stderr;
	}

	Stream
	stream_stdin()
	{
		static Internal_Stream _stdin = _stream_std(file_stdin());
		return (Stream)&_stdin;
	}

	struct Stream_Tmp_Wrapper
	{
		Internal_Stream self;

		Stream_Tmp_Wrapper()
		{
			self = Internal_Stream{};
			self.kind = Internal_Stream::KIND_MEMORY;
			self.memory = memory_stream_new(allocator_top());
		}

		~Stream_Tmp_Wrapper()
		{
			memory_stream_free(self.memory);
		}
	};

	Stream
	stream_tmp()
	{
		thread_local Stream_Tmp_Wrapper _tmp;
		return (Stream)&_tmp.self;
	}

	Stream
	stream_file_new(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode)
	{
		File file = file_open(filename, io_mode, open_mode);
		if(file_valid(file) == false)
		{
			return nullptr;
			assert(false &&
				   "Cannot open file");
		}
		
		Internal_Stream* self = _stream_pool()->get();
		self->kind = Internal_Stream::KIND_FILE;
		self->file = file;
		return (Stream)self;
	}

	Stream
	stream_memory_new(Allocator allocator)
	{
		Internal_Stream* self = _stream_pool()->get();
		self->kind = Internal_Stream::KIND_MEMORY;
		self->memory = memory_stream_new(allocator);
		return (Stream)self;
	}

	void
	stream_free(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				file_close(self->file);
				break;

			case Internal_Stream::KIND_MEMORY:
				memory_stream_free(self->memory);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		_stream_pool()->put(self);
	}

	size_t
	stream_write(Stream stream, Block data)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		size_t result = 0;

		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_write(self->file, data);
				break;

			case Internal_Stream::KIND_MEMORY:
				result = memory_stream_write(self->memory, data);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	size_t
	stream_read(Stream stream, Block data)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		size_t result = 0;

		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_read(self->file, data);
				break;

			case Internal_Stream::KIND_MEMORY:
				result = memory_stream_read(self->memory, data);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	int64_t
	stream_size(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		int64_t result = 0;

		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_size(self->file);
				break;

			case Internal_Stream::KIND_MEMORY:
				result = memory_stream_size(self->memory);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	int64_t
	stream_cursor_pos(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		int64_t result = 0;

		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_cursor_pos(self->file);
				break;

			case Internal_Stream::KIND_MEMORY:
				result = memory_stream_cursor_pos(self->memory);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	void
	stream_cursor_move(Stream stream, int64_t offset)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move(self->file, offset);
				assert(res == true && "File cursor move failed");
				break;

			case Internal_Stream::KIND_MEMORY:
				memory_stream_cursor_move(self->memory, offset);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	void
	stream_cursor_move_to_start(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move_to_start(self->file);
				assert(res == true && "File cursor move failed");
				break;

			case Internal_Stream::KIND_MEMORY:
				memory_stream_cursor_to_start(self->memory);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	void
	stream_cursor_move_to_end(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case Internal_Stream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move_to_end(self->file);
				assert(res == true && "File cursor move failed");
				break;

			case Internal_Stream::KIND_MEMORY:
				memory_stream_cursor_to_end(self->memory);
				break;

			case Internal_Stream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	const char*
	stream_str(Stream stream)
	{
		Internal_Stream* self = (Internal_Stream*)stream;

		assert(self->kind == Internal_Stream::KIND_MEMORY && "stream_str is only supported in memory streams");
		//set the byte at the cursor
		if(size_t(self->memory.cursor) == self->memory.str.count)
			buf_push(self->memory.str, '\0');
		else
			self->memory.str[self->memory.cursor] = '\0';
		return (const char*)memory_stream_block_behind(self->memory, 0).ptr;
	}
	
	//extend memory stream
	size_t
	memory_stream_pipe(Memory_Stream& self, Stream stream, size_t size)
	{
		if(self.str.count - self.cursor < size)
			memory_stream_reserve(self, size);

		size_t read_size = stream_read(stream, Block { self.str.ptr + self.cursor, size });
		self.str.count += read_size;
		self.cursor += read_size;
		return read_size;
	}

	size_t
	memory_stream_pipe(Memory_Stream& self, File file, size_t size)
	{
		if(self.str.count - self.cursor < size)
			memory_stream_reserve(self, size);

		size_t read_size = file_read(file, Block { self.str.ptr + self.cursor, size });
		self.str.count += read_size;
		self.cursor += read_size;
		return read_size;
	}
}
