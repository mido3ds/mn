#include "mn/Library.h"
#include "mn/File.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace mn
{
	inline static HMODULE
	_local_module()
	{
		static HMODULE PROCESS_MODULE = GetModuleHandle(NULL);
		return PROCESS_MODULE;
	}

	// API
	Library
	library_open(const Str& filename)
	{
		if(filename.count == 0)
			return _local_module();

		auto os_str = to_os_encoding(filename, memory::tmp());
		return LoadLibrary((LPWSTR)os_str.ptr);
	}

	void
	library_close(Library self)
	{
		if(self == _local_module())
			return;

		FreeLibrary((HMODULE)self);
	}

	void*
	library_proc(Library self, const Str& proc_name)
	{
		return GetProcAddress((HMODULE)self, proc_name.ptr);
	}
}