#pragma once

#include "mn/Exports.h"
#include "mn/Buf.h"
#include "mn/Map.h"

#include <string.h>
#include <stdio.h>
#include <utility>
#include <assert.h>

namespace mn
{
	/**
	 * A String is just a `Buf<char>` so it's suitable to use the buf functions with the string
	 * but use with caution since the null terminator should be maintained all the time
	 */
	using Str = Buf<char>;

	/**
	 * @brief      Returns a new string
	 */
	MN_EXPORT Str
	str_new();

	/**
	 * @brief      Returns a new string
	 *
	 * @param[in]  allocator  The allocator to be used by the string
	 */
	MN_EXPORT Str
	str_with_allocator(Allocator allocator);

	/**
	 * @brief      Returns a new string which has the same content as the given
	 * C string (performs a copy)
	 *
	 * @param[in]  str        The C string
	 * @param[in]  allocator  The allocator
	 */
	MN_EXPORT Str
	str_from_c(const char* str, Allocator allocator = allocator_top());

	/**
	 * @brief      Returns a new string which has the same content of the given
	 * C sub string (performs a copy)
	 *
	 * @param[in]  begin      The begin
	 * @param[in]  end        The end
	 * @param[in]  allocator  The allocator
	 */
	MN_EXPORT Str
	str_from_substr(const char* begin, const char* end, Allocator allocator = allocator_top());

	/**
	 * @brief      Just wraps the given C string literal (doesn't copy)
	 *
	 * @param[in]  lit   The C string literal
	 */
	MN_EXPORT Str
	str_lit(const char* lit);

	inline static Str
	str_tmp(const char* str = nullptr)
	{
		return str_from_c(str, memory::tmp());
	}

	/**
	 * @brief      Frees the string
	 */
	MN_EXPORT void
	str_free(Str& self);

	/**
	 * @brief      Destruct function overload for the string
	 *
	 * @param      self  The string
	 */
	inline static void
	destruct(Str& self)
	{
		str_free(self);
	}

	/**
	 * @brief      Returns the rune(utf-8 chars) count in the given string
	 *
	 * @param[in]  str   The string
	 */
	MN_EXPORT size_t
	rune_count(const char* str);

	/**
	 * @brief      Returns the size(in bytes) of the given runes
	 *
	 * @param[in]  r     The rune
	 */
	inline static size_t
	rune_size(int32_t r)
	{
		char* b = (char*)&r;
		return ((b[0] != 0) +
				(b[1] != 0) +
				(b[2] != 0) +
				(b[3] != 0));
	}

	/**
	 * @brief      Given a string iterator/pointer it will move it to point to the next rune
	 *
	 * @param[in]  str   The string
	 */
	inline static const char*
	rune_next(const char* str)
	{
		++str;
		while(*str && ((*str & 0xC0) == 0x80))
			++str;
		return str;
	}

	/**
	 * @brief      Extract a rune from the given string
	 *
	 * @param[in]  c     The string
	 */
	inline static int32_t
	rune_read(const char* c)
	{
		if(c == nullptr)
			return 0;

		if(*c == 0)
			return 0;

		int32_t rune = 0;
		uint8_t* result = (uint8_t*)&rune;
		const uint8_t* it = (const uint8_t*)c;
		*result++ = *it++;
		while (*it && ((*it & 0xC0) == 0x80))
			*result++ = *it++;
		return rune;
	}

	/**
	 * @brief      Returns the rune count of the given string
	 *
	 * @param[in]  self  The string
	 */
	inline static size_t
	str_rune_count(const Str& self)
	{
		if(self.count)
			return rune_count(self.ptr);
		return 0;
	}

	/**
	 * @brief      Pushes the second string into the first one
	 *
	 * @param      self  The first string
	 * @param[in]  str   The second string literal
	 */
	MN_EXPORT void
	str_push(Str& self, const char* str);

	/**
	 * @brief      Pushes the second string into the first one
	 *
	 * @param      self  The first string
	 * @param[in]  str   The second string
	 */
	inline static void
	str_push(Str& self, const Str& str)
	{
		str_push(self, str.ptr);
	}

	/**
	 * @brief      Pushes a block of memory into the string
	 *
	 * @param      self   The string
	 * @param[in]  block  The block
	 */
	MN_EXPORT void
	str_block_push(Str& self, Block block);

	/**
	 * @brief      uses printf family of functions to write a formatted string into the string
	 *
	 * @param      self       The string
	 * @param[in]  fmt        The format
	 * @param[in]  args       The arguments of the printf
	 */
	template<typename ... TArgs>
	inline static void
	str_pushf(Str& self, const char* fmt, TArgs&& ... args)
	{
		size_t size = self.cap - self.count;
		size_t len = ::snprintf(self.ptr + self.count, size, fmt, std::forward<TArgs>(args)...) + 1;
		if(len > size)
		{
			buf_reserve(self, len);
			size = self.cap - self.count;
			len = ::snprintf(self.ptr + self.count, size, fmt, std::forward<TArgs>(args)...) + 1;
			assert(len <= size);
		}
		self.count += len - 1;
	}

	/**
	 * @brief      Ensures that the string is null terminated
	 *
	 * @param      self  The string
	 */
	MN_EXPORT void
	str_null_terminate(Str& self);

	/**
	 * @brief      Searches for the given string
	 *
	 * @param[in]  self    The string to search in
	 * @param[in]  target  The target string to find
	 * @param[in]  start   The start index to begin the search with
	 *
	 * @return     index of the found target or size_t(-1) instead
	 */
	MN_EXPORT size_t
	str_find(const Str& self, const Str& target, size_t start);

	/**
	 * @brief      Searches for the given string
	 *
	 * @param[in]  self    The string to search in
	 * @param[in]  target  The target string to find
	 * @param[in]  start   The start index to begin the search with
	 *
	 * @return     index of the found target or size_t(-1) instead
	 */
	inline static size_t
	str_find(const Str& self, const char* target, size_t start)
	{
		return str_find(self, str_lit(target), start);
	}

	/**
	 * @brief      Searches for the given string
	 *
	 * @param[in]  self    The string to search in
	 * @param[in]  target  The target string to find
	 * @param[in]  start   The start index to begin the search with
	 *
	 * @return     index of the found target or size_t(-1) instead
	 */
	inline static size_t
	str_find(const char* self, const Str& target, size_t start)
	{
		return str_find(str_lit(self), target, start);
	}

	/**
	 * @brief      Searches for the given string
	 *
	 * @param[in]  self    The string to search in
	 * @param[in]  target  The target string to find
	 * @param[in]  start   The start index to begin the search with
	 *
	 * @return     index of the found target or size_t(-1) instead
	 */
	inline static size_t
	str_find(const char* self, const char* target, size_t start)
	{
		return str_find(str_lit(self), str_lit(target), start);
	}

	MN_EXPORT void
	str_replace(Str& self, char to_remove, char to_add);

	/**
	 * @brief      Splits the string with the given delimiter
	 *
	 * @param[in]  self        The string
	 * @param[in]  delim       The delimiter to split with
	 * @param[in]  skip_empty  determines whether we shouldn't return empty strings or not
	 * @param[in]  allocator   The allocator [optional] default is the tmp allocator
	 */
	MN_EXPORT Buf<Str>
	str_split(const Str& self, const Str& delim, bool skip_empty, Allocator allocator = memory::tmp());

	/**
	 * @brief      Splits the string with the given delimiter
	 *
	 * @param[in]  self        The string
	 * @param[in]  delim       The delimiter to split with
	 * @param[in]  skip_empty  determines whether we shouldn't return empty strings or not
	 * @param[in]  allocator   The allocator [optional] default is the tmp allocator
	 */
	inline static Buf<Str>
	str_split(const Str& self, const char* delim, bool skip_empty, Allocator allocator = memory::tmp())
	{
		return str_split(self, str_lit(delim), skip_empty, allocator);
	}

	/**
	 * @brief      Splits the string with the given delimiter
	 *
	 * @param[in]  self        The string
	 * @param[in]  delim       The delimiter to split with
	 * @param[in]  skip_empty  determines whether we shouldn't return empty strings or not
	 * @param[in]  allocator   The allocator [optional] default is the tmp allocator
	 */
	inline static Buf<Str>
	str_split(const char* self, const Str& delim, bool skip_empty, Allocator allocator = memory::tmp())
	{
		return str_split(str_lit(self), delim, skip_empty, allocator);
	}

	/**
	 * @brief      Splits the string with the given delimiter
	 *
	 * @param[in]  self        The string
	 * @param[in]  delim       The delimiter to split with
	 * @param[in]  skip_empty  determines whether we shouldn't return empty strings or not
	 * @param[in]  allocator   The allocator [optional] default is the tmp allocator
	 */
	inline static Buf<Str>
	str_split(const char* self, const char* delim, bool skip_empty, Allocator allocator = memory::tmp())
	{
		return str_split(str_lit(self), str_lit(delim), skip_empty, allocator);
	}

	/**
	 * @brief      Checks if the string is starting with the given prefix
	 *
	 * @param[in]  self    The string
	 * @param[in]  prefix  The prefix
	 */
	MN_EXPORT bool
	str_prefix(const Str& self, const Str& prefix);

	/**
	 * @brief      Checks if the string is starting with the given prefix
	 *
	 * @param[in]  self    The string
	 * @param[in]  prefix  The prefix
	 */
	inline static bool
	str_prefix(const Str& self, const char* prefix)
	{
		return str_prefix(self, str_lit(prefix));
	}

	/**
	 * @brief      Checks if the string is starting with the given prefix
	 *
	 * @param[in]  self    The string
	 * @param[in]  prefix  The prefix
	 */
	inline static bool
	str_prefix(const char* self, const Str& prefix)
	{
		return str_prefix(str_lit(self), prefix);
	}

	/**
	 * @brief      Checks if the string is starting with the given prefix
	 *
	 * @param[in]  self    The string
	 * @param[in]  prefix  The prefix
	 */
	inline static bool
	str_prefix(const char* self, const char* prefix)
	{
		return str_prefix(str_lit(self), str_lit(prefix));
	}

	/**
	 * @brief      Checks if the string is ending with the given suffix
	 *
	 * @param[in]  self    The string
	 * @param[in]  suffix  The suffix
	 */
	MN_EXPORT bool
	str_suffix(const Str& self, const Str& suffix);

	/**
	 * @brief      Checks if the string is ending with the given suffix
	 *
	 * @param[in]  self    The string
	 * @param[in]  suffix  The suffix
	 */
	inline static bool
	str_suffix(const Str& self, const char* suffix)
	{
		return str_suffix(self, str_lit(suffix));
	}

	/**
	 * @brief      Checks if the string is ending with the given suffix
	 *
	 * @param[in]  self    The string
	 * @param[in]  suffix  The suffix
	 */
	inline static bool
	str_suffix(const char* self, const Str& suffix)
	{
		return str_suffix(str_lit(self), suffix);
	}

	/**
	 * @brief      Checks if the string is ending with the given suffix
	 *
	 * @param[in]  self    The string
	 * @param[in]  suffix  The suffix
	 */
	inline static bool
	str_suffix(const char* self, const char* suffix)
	{
		return str_suffix(str_lit(self), str_lit(suffix));
	}

	/**
	 * @brief      Resizes the string to the given size
	 *
	 * @param      self  The string
	 * @param[in]  size  The size
	 */
	MN_EXPORT void
	str_resize(Str& self, size_t size);

	/**
	 * @brief      Clears the string
	 *
	 * @param      self  The string
	 */
	MN_EXPORT void
	str_clear(Str& self);

	/**
	 * @brief      Clones the given string
	 *
	 * @param[in]  other      The string
	 * @param[in]  allocator  The allocator to be used in the returned string
	 *
	 * @return     The newly cloned string
	 */
	MN_EXPORT Str
	str_clone(const Str& other, Allocator allocator = allocator_top());

	inline static bool
	str_empty(const Str& self)
	{
		return self.count == 0;
	}

	/**
	 * @brief      Clone function overload for the string type
	 *
	 * @param[in]  other  The string to be cloned
	 *
	 * @return     Copy of this object.
	 */
	inline static Str
	clone(const Str& other)
	{
		return str_clone(other);
	}

	template<>
	struct Hash<Str>
	{
		inline size_t
		operator()(const Str& str) const
		{
			return str.count ? murmur_hash(str.ptr, str.count) : 0;
		}
	};

	inline static int
	str_cmp(const char* a, const char* b)
	{
		if (a != nullptr && b != nullptr)
		{
			return ::strcmp(a, b);
		}
		else if (a == nullptr && b == nullptr)
		{
			return 0;
		}
		else if (a != nullptr && b == nullptr)
		{
			if (::strlen(a) == 0)
				return 0;
			else
				return 1;
		}
		else if (a == nullptr && b != nullptr)
		{
			if (::strlen(b) == 0)
				return 0;
			else
				return -1;
		}
		else
		{
			assert(false && "UNREACHABLE");
			return 0;
		}
	}

	inline static bool
	operator==(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) == 0;
	}

	inline static bool
	operator!=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) != 0;
	}

	inline static bool
	operator<(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) < 0;
	}

	inline static bool
	operator<=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) <= 0;
	}

	inline static bool
	operator>(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) > 0;
	}

	inline static bool
	operator>=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) >= 0;
	}


	inline static bool
	operator==(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) == 0;
	}

	inline static bool
	operator!=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) != 0;
	}

	inline static bool
	operator<(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) < 0;
	}

	inline static bool
	operator<=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) <= 0;
	}

	inline static bool
	operator>(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) > 0;
	}

	inline static bool
	operator>=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) >= 0;
	}


	inline static bool
	operator==(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) == 0;
	}

	inline static bool
	operator!=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) != 0;
	}

	inline static bool
	operator<(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) < 0;
	}

	inline static bool
	operator<=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) <= 0;
	}

	inline static bool
	operator>(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) > 0;
	}

	inline static bool
	operator>=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) >= 0;
	}
}
