#pragma once

#include <mn/Memory_Stream.h>

#include <stdint.h>
#include <assert.h>

namespace mn
{
	using Bytes = Memory_Stream;

	inline static Bytes
	bytes_new()
	{
		return memory_stream_new();
	}

	inline static Bytes
	bytes_with_allocator(Allocator allocator)
	{
		return memory_stream_new(allocator);
	}

	inline static void
	bytes_free(Bytes& self)
	{
		memory_stream_free(self);
	}

	inline static void
	destruct(Bytes& self)
	{
		bytes_free(self);
	}

	inline static size_t
	bytes_size(const Bytes& self)
	{
		return memory_stream_size(self);
	}

	inline static bool
	bytes_eof(const Bytes& self)
	{
		return memory_stream_eof(self);
	}

	inline static void
	bytes_concat(Bytes& self, const Bytes& other)
	{
		memory_stream_write(self, block_from(other.str));
	}

	inline static void
	bytes_rewind(Bytes& self)
	{
		memory_stream_cursor_to_start(self);
	}

	inline static int64_t
	bytes_cursor_pos(const Bytes& self)
	{
		return memory_stream_cursor_pos(self);
	}

	inline static void
	bytes_cursor_move(Bytes& self, int64_t offset)
	{
		memory_stream_cursor_move(self, offset);
	}

	inline static void
	bytes_cursor_set(Bytes& self, int64_t absolute)
	{
		memory_stream_cursor_set(self, absolute);
	}

	inline static void
	bytes_push8(Bytes& self, uint8_t v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push16(Bytes& self, uint16_t v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push32(Bytes& self, uint32_t v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push64(Bytes& self, uint64_t v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push32f(Bytes& self, float v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push64f(Bytes& self, double v)
	{
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}

	inline static void
	bytes_push_ptr(Bytes& self, void* ptr)
	{
		uintptr_t v = (uintptr_t)ptr;
		[[maybe_unused]] size_t written_size = memory_stream_write(self, block_from(v));
		assert(written_size == sizeof(v));
	}


	inline static uint8_t
	bytes_pop8(Bytes& self)
	{
		uint8_t res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static uint16_t
	bytes_pop16(Bytes& self)
	{
		uint16_t res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static uint32_t
	bytes_pop32(Bytes& self)
	{
		uint32_t res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static uint64_t
	bytes_pop64(Bytes& self)
	{
		uint64_t res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static float
	bytes_pop32f(Bytes& self)
	{
		float res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static double
	bytes_pop64f(Bytes& self)
	{
		double res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}

	inline static void*
	bytes_pop_ptr(Bytes& self)
	{
		void* res = 0;
		[[maybe_unused]] size_t read_size = memory_stream_read(self, block_from(res));
		assert(read_size == sizeof(res));
		return res;
	}
}
