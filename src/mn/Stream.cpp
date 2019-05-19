#include "mn/Stream.h"
#include "mn/Memory_Stream.h"
#include "mn/Pool.h"

//to supress warnings in release mode
#define UNUSED(x) ((void)(x))

namespace mn
{
	struct IStream
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
	TS_Typed_Pool<IStream>*
	_stream_pool()
	{
		static TS_Typed_Pool<IStream> _pool(1024, memory::clib());
		return &_pool;
	}

	IStream
	_stream_std(File file)
	{
		IStream self{};
		self.kind = IStream::KIND_FILE;
		self.file = file;
		return self;
	}

	Stream
	stream_stdout()
	{
		static IStream _stdout = _stream_std(file_stdout());
		return &_stdout;
	}

	Stream
	stream_stderr()
	{
		static IStream _stderr = _stream_std(file_stderr());
		return &_stderr;
	}

	Stream
	stream_stdin()
	{
		static IStream _stdin = _stream_std(file_stdin());
		return &_stdin;
	}

	struct Stream_Tmp_Wrapper
	{
		IStream self;

		Stream_Tmp_Wrapper()
		{
			self = IStream{};
			self.kind = IStream::KIND_MEMORY;
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
		return &_tmp.self;
	}

	Stream
	stream_file_new(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode)
	{
		File file = file_open(filename, io_mode, open_mode);
		if(file_valid(file) == false)
		{
			assert(false && "Cannot open file");
			return nullptr;
		}
		
		Stream self = _stream_pool()->get();
		self->kind = IStream::KIND_FILE;
		self->file = file;
		return self;
	}

	Stream
	stream_memory_new(Allocator allocator)
	{
		Stream self = _stream_pool()->get();
		self->kind = IStream::KIND_MEMORY;
		self->memory = memory_stream_new(allocator);
		return self;
	}

	void
	stream_free(Stream self)
	{
		switch (self->kind)
		{
			case IStream::KIND_FILE:
				file_close(self->file);
				break;

			case IStream::KIND_MEMORY:
				memory_stream_free(self->memory);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		_stream_pool()->put(self);
	}

	size_t
	stream_write(Stream self, Block data)
	{
		size_t result = 0;

		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_write(self->file, data);
				break;

			case IStream::KIND_MEMORY:
				result = memory_stream_write(self->memory, data);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	size_t
	stream_read(Stream self, Block data)
	{
		size_t result = 0;

		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_read(self->file, data);
				break;

			case IStream::KIND_MEMORY:
				result = memory_stream_read(self->memory, data);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	int64_t
	stream_size(Stream self)
	{
		int64_t result = 0;

		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_size(self->file);
				break;

			case IStream::KIND_MEMORY:
				result = memory_stream_size(self->memory);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	int64_t
	stream_cursor_pos(Stream self)
	{
		int64_t result = 0;

		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				result = file_cursor_pos(self->file);
				break;

			case IStream::KIND_MEMORY:
				result = memory_stream_cursor_pos(self->memory);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
		return result;
	}

	void
	stream_cursor_move(Stream self, int64_t offset)
	{
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move(self->file, offset);
				assert(res == true && "File cursor move failed");
				break;

			case IStream::KIND_MEMORY:
				memory_stream_cursor_move(self->memory, offset);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	void
	stream_cursor_move_to_start(Stream self)
	{
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move_to_start(self->file);
				assert(res == true && "File cursor move failed");
				break;

			case IStream::KIND_MEMORY:
				memory_stream_cursor_to_start(self->memory);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	void
	stream_cursor_move_to_end(Stream self)
	{
		bool res = false;
		UNUSED(res);
		switch (self->kind)
		{
			case IStream::KIND_FILE:
				assert(file_valid(self->file) && "Invalid file");
				res = file_cursor_move_to_end(self->file);
				assert(res == true && "File cursor move failed");
				break;

			case IStream::KIND_MEMORY:
				memory_stream_cursor_to_end(self->memory);
				break;

			case IStream::KIND_NONE:
			default:
				assert(false &&
					   "Invalid stream type");
				break;
		}
	}

	const char*
	stream_str(Stream self)
	{
		assert(self->kind == IStream::KIND_MEMORY && "stream_str is only supported in memory streams");
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
