#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn
{
	typedef int32_t Rune;

	MN_EXPORT size_t
	rune_count(const char* str);

	MN_EXPORT Rune
	rune_lower(Rune c);

	MN_EXPORT Rune
	rune_upper(Rune c);

	MN_EXPORT int
	rune_size(Rune c);

	MN_EXPORT bool
	rune_is_letter(Rune c);

	MN_EXPORT bool
	rune_is_number(Rune c);

	MN_EXPORT const char*
	rune_next(const char* str);

	MN_EXPORT const char*
	rune_prev(const char* str);

	MN_EXPORT Rune
	rune_read(const char* c);

	MN_EXPORT bool
	rune_valid(Rune c);

	// rune_encode encodes the given rune into the provided block of memory, note that the block size must be >= 4
	// it returns the encoded size in bytes
	MN_EXPORT size_t
	rune_encode(Rune c, Block b);
}