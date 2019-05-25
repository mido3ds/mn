#pragma once

#include <mn/Buf.h>

#include <stdint.h>
#include <assert.h>

namespace mn
{
	using Bytes = Buf<uint8_t>;

	inline static Bytes
	bytes_new()
	{
		return buf_new<uint8_t>();
	}

	inline static Bytes
	bytes_with_allocator(Allocator allocator)
	{
		return buf_with_allocator<uint8_t>(allocator);
	}

	inline static void
	bytes_free(Bytes& self)
	{
		buf_free(self);
	}

	inline static void
	destruct(Bytes& self)
	{
		bytes_free(self);
	}

	inline static void
	bytes_push8(Bytes& self, uint8_t v)
	{
		buf_push(self, v);
	}

	inline static void
	bytes_push16(Bytes& self, uint16_t v)
	{
		buf_push(self, uint8_t(v));
		buf_push(self, uint8_t(v >> 8));
	}

	inline static void
	bytes_push32(Bytes& self, uint32_t v)
	{
		buf_push(self, uint8_t(v));
		buf_push(self, uint8_t(v >> 8));
		buf_push(self, uint8_t(v >> 16));
		buf_push(self, uint8_t(v >> 24));
	}

	inline static void
	bytes_push64(Bytes& self, uint64_t v)
	{
		buf_push(self, uint8_t(v));
		buf_push(self, uint8_t(v >> 8));
		buf_push(self, uint8_t(v >> 16));
		buf_push(self, uint8_t(v >> 24));
		buf_push(self, uint8_t(v >> 32));
		buf_push(self, uint8_t(v >> 40));
		buf_push(self, uint8_t(v >> 48));
		buf_push(self, uint8_t(v >> 56));
	}

	inline static void
	bytes_push32f(Bytes& self, float v)
	{
		bytes_push32(self, *(uint32_t*)&v);
	}

	inline static void
	bytes_push64f(Bytes& self, double v)
	{
		bytes_push64(self, *(uint64_t*)&v);
	}

	inline static uint8_t
	bytes_pop8(const Bytes& self, uint64_t& ix)
	{
		assert(ix + sizeof(uint8_t) <= self.count);
		uint8_t v = *(uint8_t*)(self.ptr + ix);
		ix += sizeof(v);
		return v;
	}

	inline static uint16_t
	bytes_pop16(const Bytes& self, uint64_t& ix)
	{
		assert(ix + sizeof(uint16_t) <= self.count);
		uint16_t v = *(uint16_t*)(self.ptr + ix);
		ix += sizeof(v);
		return v;
	}

	inline static uint32_t
	bytes_pop32(const Bytes& self, uint64_t& ix)
	{
		assert(ix + sizeof(uint32_t) <= self.count);
		uint32_t v = *(uint32_t*)(self.ptr + ix);
		ix += sizeof(v);
		return v;
	}

	inline static uint64_t
	bytes_pop64(const Bytes& self, uint64_t& ix)
	{
		assert(ix + sizeof(uint64_t) <= self.count);
		uint64_t v = *(uint64_t*)(self.ptr + ix);
		ix += sizeof(v);
		return v;
	}

	inline static float
	bytes_pop32f(const Bytes& self, uint64_t& ix)
	{
		uint32_t v = bytes_pop32(self, ix);
		return *(float*)&v;
	}

	inline static double
	bytes_pop64f(const Bytes& self, uint64_t& ix)
	{
		uint64_t v = bytes_pop64(self, ix);
		return *(double*)&v;
	}
}
