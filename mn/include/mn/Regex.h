#pragma once

#include "mn/Exports.h"
#include "mn/Buf.h"
#include "mn/Str.h"
#include "mn/Result.h"

namespace mn
{
	// A Simple Regex Engine
	// this is a simple implementation of a regex engine which contains all the regex operators
	// that i need and use, and they are
	// '.': matches any rune
	// '|': or operator
	// '*': zero or more operator along with the non greedy variant '*?'
	// '+': one or more operator along with the non greedy variant '+?'
	// '?': optional operator along with the non greedy variant '??'
	// '[]': in-set operator along with the not-in-set '[^]' and ranges '[a-z]'

	// defines the operators that's supported by the regex virtual machine
	enum RGX_OP: uint8_t
	{
		// [RUNE, 'a']: matches a rune
		RGX_OP_RUNE,
		// [ANY]: matches any rune
		RGX_OP_ANY,
		// [SPLIT, offset(int32_t), offset(int32_t)]: splits execution into 2 threads
		RGX_OP_SPLIT,
		// [JUMP, offset(int32_t)]: changes the instruction pointer
		RGX_OP_JUMP,
		// [SET, count(int32_t), (RANGE, BYTE)]: matches any byte that's inside the given set of options
		RGX_OP_SET,
		// [NOT_SET, count(int32_t), (RANGE, BYTE)]: matches any byte that's outside the given set of options
		RGX_OP_NOT_SET,
		// [RANGE, 'a', 'z']: specifies a range of bytes
		RGX_OP_RANGE,
		// [MATCH]: registers a match
		RGX_OP_MATCH,
		// [MATCH2, data(int32_t)]: registers a match with an int, this is used in case you have multiple matches and want
		// to make know which match did succeed
		RGX_OP_MATCH2,
	};

	// a compiled regex
	struct Regex
	{
		Buf<uint8_t> bytes;
	};

	// creates a new empty regex program
	inline static Regex
	regex_new()
	{
		return Regex{};
	}

	// frees a regex program
	inline static void
	regex_free(Regex& self)
	{
		buf_free(self.bytes);
	}

	inline static void
	destruct(Regex& self)
	{
		regex_free(self);
	}

	// clones a regex program
	inline static Regex
	regex_clone(const Regex& other, Allocator allocator = allocator_top())
	{
		return Regex{ buf_memcpy_clone(other.bytes, allocator) };
	}

	inline static Regex
	clone(const Regex& other)
	{
		return regex_clone(other);
	}

	// regex compiler
	struct Regex_Compile_Unit
	{
		Allocator program_allocator;
		Str str;
		bool enable_payload;
		int32_t payload;
	};

	// compiles a regular expression into a regex program
	MN_EXPORT Result<Regex>
	regex_compile(Regex_Compile_Unit unit);

	// compiles a regular expression into a regex program with a match2 opcode at the end
	// this is useful in case you want to identify which match did happen, in case you have
	// different matches like (abc|def) and want to identify whether the match is abc or def
	inline static Result<Regex>
	regex_compile_with_payload(const Str& str, int32_t payload, Allocator allocator = allocator_top())
	{
		Regex_Compile_Unit unit{};
		unit.program_allocator = allocator;
		unit.str = str;
		unit.enable_payload = true;
		unit.payload = payload;
		return regex_compile(unit);
	}

	inline static Result<Regex>
	regex_compile_with_payload(const char* str, int32_t payload, Allocator allocator = allocator_top())
	{
		return regex_compile_with_payload(str_lit(str), payload, allocator);
	}

	// compiles a regular expression into a regex program
	inline static Result<Regex>
	regex_compile(const Str& str, Allocator allocator = allocator_top())
	{
		Regex_Compile_Unit unit{};
		unit.program_allocator = allocator;
		unit.str = str;
		return regex_compile(unit);
	}

	inline static Result<Regex>
	regex_compile(const char* str, Allocator allocator = allocator_top())
	{
		return regex_compile(str_lit(str), allocator);
	}

	// regex vm

	// match result contains the range of the match in the string and whether it's a match or not
	// because in case of mismatch the begin, end pointers will have how far has the VM has looked
	// which means that you can discard this sub string because it won't match, this is used in search
	// also we have the payload thing mentioned above
	struct Match_Result
	{
		const char* begin;
		const char* end;
		bool match;
		bool with_payload;
		int32_t payload;
	};

	// tries to match the compiled regex program to the given string
	MN_EXPORT Match_Result
	regex_match(const Regex& program, const char* str);

	// search for the first match of the regex program in the given string
	MN_EXPORT Match_Result
	regex_search(const Regex& program, const char* str);
}