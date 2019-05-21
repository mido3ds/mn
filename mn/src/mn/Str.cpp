#include "mn/Str.h"

#include <assert.h>

namespace mn
{
	Str
	str_new()
	{
		return buf_new<char>();
	}

	Str
	str_with_allocator(Allocator allocator)
	{
		return buf_with_allocator<char>(allocator);
	}

	Str
	str_from_c(const char* str, Allocator allocator)
	{
		if (str == nullptr) return str_with_allocator(allocator);
		Str self = str_with_allocator(allocator);
		buf_resize(self, ::strlen(str) + 1);
		--self.count;
		::memcpy(self.ptr, str, self.count);
		self.ptr[self.count] = '\0';
		return self;
	}

	Str
	str_from_substr(const char* begin, const char* end, Allocator allocator)
	{
		assert(end >= begin &&
			   "Invalid substring");

		Str self = str_with_allocator(allocator);

		//this is an empty string
		if(end == begin)
			return self;

		buf_resize(self, (end - begin) + 1);
		--self.count;
		::memcpy(self.ptr, begin, self.count);
		self.ptr[self.count] = '\0';
		return self;
	}

	Str
	str_lit(const char* lit)
	{
		Str self{};
		self.ptr = (char*)lit;
		if(lit)
		{
			self.cap = ::strlen(lit) + 1;
			self.count = self.cap - 1;
		}
		return self;
	}

	void
	str_free(Str& self)
	{
		buf_free(self);
	}

	size_t
	rune_count(const char* str)
	{
		size_t result = 0;
		while(str != nullptr && *str != '\0')
		{
			result += ((*str & 0xC0) != 0x80);
			++str;
		}
		return result;
	}

	void
	str_push(Str& self, const char* str)
	{
		size_t str_len = ::strlen(str);
		size_t self_len = self.count;
		buf_resize(self, self.count + str_len + 1);
		--self.count;
		::memcpy(self.ptr + self_len, str, str_len);
		self.ptr[self.count] = '\0';
	}

	void
	str_block_push(Str& self, Block block)
	{
		size_t self_len = self.count;
		buf_resize(self, self.count + block.size + 1);
		--self.count;
		::memcpy(self.ptr + self_len, block.ptr, block.size);
		self.ptr[self.count] = '\0';
	}

	void
	str_null_terminate(Str& self)
	{
		if (self.count == 0)
			return;
		buf_reserve(self, 1);
		self.ptr[self.count] = '\0';
	}

	size_t
	str_find(const Str& self, const Str& target, size_t start)
	{
		assert(start < self.count);

		for (size_t i = start; i <= self.count - target.count; ++i)
			if (::memcmp(self.ptr + i, target.ptr, target.count) == 0)
				return i;

		return size_t(-1);
	}

	Buf<Str>
	str_split(const Str& self, const Str& delim, bool skip_empty, Allocator allocator)
	{
		Buf<Str> result = buf_with_allocator<Str>(allocator);

		size_t current_index = 0;

		while (true)
		{
			if (current_index + delim.count > self.count)
				break;

			size_t delim_index = str_find(self, delim, current_index);

			if (delim_index == size_t(-1))
				break;

			bool skip = skip_empty && current_index == delim_index;

			if (!skip)
				buf_push(result, str_from_substr(self.ptr + current_index, self.ptr + delim_index, allocator));

			current_index = delim_index + delim.count;

			if (current_index == self.count)
				break;
		}

		if (current_index != self.count)
			buf_push(result, str_from_substr(self.ptr + current_index, self.ptr + self.count, allocator));
		else if (!skip_empty && current_index == self.count)
			buf_push(result, str_with_allocator(allocator));

		return result;
	}

	bool
	str_prefix(const Str& self, const Str& prefix)
	{
		if(self.count < prefix.count)
			return false;
		return ::memcmp(self.ptr, prefix.ptr, prefix.count) == 0;
	}

	bool
	str_suffix(const Str& self, const Str& suffix)
	{
		if(self.count < suffix.count)
			return false;
		return ::memcmp(self.ptr + self.count - suffix.count, suffix.ptr, suffix.count) == 0;
	}

	void
	str_resize(Str& self, size_t size)
	{
		buf_resize(self, size + 1);
		--self.count;
		self.ptr[self.count] = '\0';
	}

	void
	str_clear(Str& self)
	{
		buf_clear(self);
	}

	Str
	str_clone(const Str& other, Allocator allocator)
	{
		Str self = str_with_allocator(allocator);
		buf_resize(self, other.count + 1);
		--self.count;
		::memcpy(self.ptr, other.ptr, self.count);
		self.ptr[self.count] = '\0';
		return self;
	}
}