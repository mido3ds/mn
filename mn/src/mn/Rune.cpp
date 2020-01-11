#include "mn/Rune.h"

#include "utf8proc/utf8proc.h"

#include <assert.h>

namespace mn
{
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

	Rune
	rune_lower(Rune c)
	{
		return utf8proc_tolower(c);
	}

	Rune
	rune_upper(Rune c)
	{
		return utf8proc_toupper(c);
	}

	int
	rune_size(Rune c)
	{
		return utf8proc_charwidth(c);
	}

	bool
	rune_is_letter(Rune c)
	{
		utf8proc_category_t cat = utf8proc_category(c);
		return (
			cat == UTF8PROC_CATEGORY_LU ||
			cat == UTF8PROC_CATEGORY_LL ||
			cat == UTF8PROC_CATEGORY_LT ||
			cat == UTF8PROC_CATEGORY_LM ||
			cat == UTF8PROC_CATEGORY_LO
		);
	}

	bool
	rune_is_number(Rune c)
	{
		utf8proc_category_t cat = utf8proc_category(c);
		return (
			cat == UTF8PROC_CATEGORY_ND ||
			cat == UTF8PROC_CATEGORY_NL ||
			cat == UTF8PROC_CATEGORY_NO
		);
	}

	const char*
	rune_next(const char* str)
	{
		++str;
		while(*str && ((*str & 0xC0) == 0x80))
			++str;
		return str;
	}

	const char*
	rune_prev(const char* str)
	{
		--str;
		while(*str && ((*str & 0xC0) == 0x80))
			--str;
		return str;
	}

	Rune
	rune_read(const char* c)
	{
		if(c == nullptr)
			return 0;

		if(*c == 0)
			return 0;

		size_t str_count = 1;
		for(size_t i = 1; i < 4; ++i)
		{
			if(c[i] == 0)
				break;
			else
				++str_count;
		}

		Rune r = 0;
		utf8proc_iterate((utf8proc_uint8_t*)c, str_count, (utf8proc_int32_t*)&r);
		return r;
	}

	bool
	rune_valid(Rune c)
	{
		return utf8proc_codepoint_valid((utf8proc_int32_t)c);
	}

	size_t
	rune_encode(Rune c, Block b)
	{
		assert(b.size >= 4);
		return utf8proc_encode_char(c, (utf8proc_uint8_t*)b.ptr);
	}
}