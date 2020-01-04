#pragma once

#include "mn/Exports.h"
#include "mn/Str.h"

namespace mn
{
	typedef void* Library;

	MN_EXPORT Library
	library_open(const Str& filename);

	inline static Library
	library_open(const char* filename)
	{
		return library_open(str_lit(filename));
	}

	MN_EXPORT void
	library_close(Library self);

	inline static void
	destruct(Library self)
	{
		library_close(self);
	}

	MN_EXPORT void*
	library_proc(Library self, const Str& proc_name);

	inline static void*
	library_proc(Library self, const char* proc_name)
	{
		return library_proc(self, str_lit(proc_name));
	}
}