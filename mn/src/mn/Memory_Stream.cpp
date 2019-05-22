#include "mn/Memory_Stream.h"

namespace mn
{
	Memory_Stream
	memory_stream_new(Allocator allocator)
	{
		return Memory_Stream { str_with_allocator(allocator), 0 };
	}

	void
	memory_stream_free(Memory_Stream& self)
	{
		str_free(self.str);
	}

	size_t
	memory_stream_write(Memory_Stream& self, Block data)
	{
		assert(self.cursor >= 0 &&
			   "Memory_Stream cursor is not valid");
		buf_resize(self.str, self.cursor + data.size);
		::memcpy(self.str.ptr + self.cursor, data.ptr, data.size);
		self.cursor += data.size;
		return data.size;
	}

	size_t
	memory_stream_read(Memory_Stream& self, Block data)
	{
		assert(self.cursor >= 0 &&
			   "Memory_Stream cursor is not valid");
		assert(self.cursor <= int64_t(self.str.count) &&
			   "Memory_Stream cursor is not valid");
		size_t available_size = self.str.count - self.cursor;
		if(available_size)
		{
			available_size = available_size > data.size ? data.size : available_size;
			::memcpy(data.ptr, self.str.ptr + self.cursor, available_size);
			self.cursor += available_size;
		}
		return available_size;
	}

	int64_t
	memory_stream_size(const Memory_Stream& self)
	{
		return self.str.count;
	}

	bool
	memory_stream_eof(const Memory_Stream& self)
	{
		return self.cursor >= int64_t(self.str.count);
	}

	int64_t
	memory_stream_cursor_pos(Memory_Stream& self)
	{
		return self.cursor;
	}

	void
	memory_stream_cursor_move(Memory_Stream& self, int64_t offset)
	{
		assert(self.cursor + offset >= 0 &&
			   "Memory_Stream cursor is not valid");
		assert(size_t(self.cursor + offset) <= self.str.count &&
			   "Memory_Stream cursor is not valid");
		self.cursor += offset;
	}

	void
	memory_stream_cursor_set(Memory_Stream& self, int64_t abs)
	{
		assert(abs >= 0);
		assert(size_t(abs) <= self.str.count);
		self.cursor = abs;
	}

	void
	memory_stream_cursor_to_start(Memory_Stream& self)
	{
		self.cursor = 0;
	}

	void
	memory_stream_cursor_to_end(Memory_Stream& self)
	{
		self.cursor = self.str.count;
	}

	void
	memory_stream_reserve(Memory_Stream& self, size_t size)
	{
		buf_reserve(self.str, size);
	}

	size_t
	memory_stream_capacity(Memory_Stream& self)
	{
		return self.str.cap;
	}

	void
	memory_stream_clear(Memory_Stream& self)
	{
		str_clear(self.str);
		self.cursor = 0;
	}

	Block
	memory_stream_block_ahead(Memory_Stream& self, size_t size)
	{
		if(size == 0)
			size = self.str.count - self.cursor;
		assert(size <= self.str.count - self.cursor);
		return Block { self.str.ptr + self.cursor, size };
	}

	Block
	memory_stream_block_behind(Memory_Stream& self, size_t size)
	{
		if(size == 0)
			size = self.cursor;
		assert(size <= size_t(self.cursor));
		return Block { self.str.ptr, size };
	}
}
