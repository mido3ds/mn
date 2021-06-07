#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

#include <stdint.h>
#include <stddef.h>

namespace mn
{
	// a rune, which is a utf-8 character
	typedef int32_t Rune;

	// returns the count of runes in a string
	MN_EXPORT size_t
	rune_count(const char* str);

	// converts a rune to lower case
	MN_EXPORT Rune
	rune_lower(Rune c);

	// converts a rune to upper case
	MN_EXPORT Rune
	rune_upper(Rune c);

	// returns the size of the rune in bytes
	MN_EXPORT int
	rune_size(Rune c);

	// return whether the given rune is a letter
	MN_EXPORT bool
	rune_is_letter(Rune c);

	// return whether the given rune is a number
	MN_EXPORT bool
	rune_is_number(Rune c);

	// moves the given pointer to the next rune
	MN_EXPORT const char*
	rune_next(const char* str);

	// moves the given pointer back to the previous rune
	MN_EXPORT const char*
	rune_prev(const char* str);

	// reads a single rune off the given pointer
	MN_EXPORT Rune
	rune_read(const char* c);

	// checks whether the given rune is valid
	MN_EXPORT bool
	rune_valid(Rune c);

	// encodes the given rune into the provided block of memory, note that the block size must be >= 4
	// it returns the encoded size in bytes
	MN_EXPORT size_t
	rune_encode(Rune c, Block b);
}