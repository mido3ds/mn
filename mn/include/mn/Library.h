#pragma once

#include "mn/Exports.h"
#include "mn/Str.h"

namespace mn
{
	// a library handle
	typedef void* Library;

	// tries to open the library at the given path, if it fails it will return a nullptr
	MN_EXPORT Library
	library_open(const Str& filename);

	// tries to open the library at the given path, if it fails it will return a nullptr
	inline static Library
	library_open(const char* filename)
	{
		return library_open(str_lit(filename));
	}

	// closes the given library
	MN_EXPORT void
	library_close(Library self);

	// destruct overload for library close
	inline static void
	destruct(Library self)
	{
		library_close(self);
	}

	// searches for a procedure with the given name inside the library
	MN_EXPORT void*
	library_proc(Library self, const Str& proc_name);

	// searches for a procedure with the given name inside the library
	inline static void*
	library_proc(Library self, const char* proc_name)
	{
		return library_proc(self, str_lit(proc_name));
	}
}