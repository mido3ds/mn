#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Str.h"

#include <stdint.h>

namespace mn
{
	enum STREAM_CURSOR_OP
	{
		STREAM_CURSOR_GET,
		STREAM_CURSOR_MOVE,
		STREAM_CURSOR_SET,
		STREAM_CURSOR_START,
		STREAM_CURSOR_END,
	};
	constexpr inline int64_t STREAM_CURSOR_ERROR = INT64_MIN;

	typedef struct IStream* Stream;
	struct IStream
	{
		virtual void dispose() = 0;
		virtual size_t read(Block data) = 0;
		virtual size_t write(Block data) = 0;
		virtual int64_t size() = 0;
		virtual int64_t cursor_operation(STREAM_CURSOR_OP op, int64_t offset) = 0;
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

	inline static int64_t
	stream_cursor_pos(IStream* self)
	{
		return self->cursor_operation(STREAM_CURSOR_GET, 0);
	}

	inline static int64_t
	stream_cursor_move(IStream* self, int64_t offset)
	{
		return self->cursor_operation(STREAM_CURSOR_MOVE, offset);
	}

	inline static int64_t
	stream_cursor_set(IStream* self, int64_t abs)
	{
		return self->cursor_operation(STREAM_CURSOR_SET, abs);
	}

	inline static int64_t
	stream_cursor_to_start(IStream* self)
	{
		return self->cursor_operation(STREAM_CURSOR_START, 0);
	}

	inline static int64_t
	stream_cursor_to_end(IStream* self)
	{
		return self->cursor_operation(STREAM_CURSOR_END, 0);
	}

	inline static size_t
	stream_copy(IStream* dst, IStream* src)
	{
		size_t res = 0;
		char _buf[1024];
		auto buf = block_from(_buf);
		while(true)
		{
			auto read_size = src->read(buf);
			if (read_size == 0)
				break;

			auto ptr = (char*)buf.ptr;
			auto size = read_size;
			while (size > 0)
			{
				auto write_size = dst->write(Block{ptr, size});
				if (write_size == 0)
					return res;
				size -= write_size;
				ptr += write_size;
				res += write_size;
			}
		}
		return res;
	}

	inline static size_t
	stream_copy(Block dst, IStream* src)
	{
		size_t res = 0;
		auto ptr = (char*)dst.ptr;
		auto size = dst.size;
		while(size > 0)
		{
			auto read_size = src->read(Block{ptr, size});
			if (read_size == 0)
				break;

			ptr += read_size;
			size -= read_size;
			res += read_size;
		}
		return res;
	}

	inline static size_t
	stream_copy(IStream* dst, Block src)
	{
		size_t res = 0;
		auto ptr = (char*)src.ptr;
		auto size = src.size;
		while(size > 0)
		{
			auto write_size = dst->write(Block{ptr, size});
			if (write_size == 0)
				break;

			ptr += write_size;
			size -= write_size;
			res += write_size;
		}
		return res;
	}

	inline static Str
	stream_sink(IStream* src, Allocator allocator = allocator_top())
	{
		auto res = str_with_allocator(allocator);
		char _buf[1024];
		auto buf = block_from(_buf);
		while(true)
		{
			auto read_size = src->read(buf);
			if (read_size == 0)
				break;

			str_block_push(res, Block{buf.ptr, read_size});
		}
		return res;
	}
}