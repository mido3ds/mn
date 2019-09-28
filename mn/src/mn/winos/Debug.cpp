#include "mn/Debug.h"
#include "mn/Str.h"

#include <string.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#include <DbgHelp.h>

namespace mn
{
	struct Debugger_Callstack
	{
		Debugger_Callstack()
		{
			SymInitialize(GetCurrentProcess(), NULL, true);
		}

		~Debugger_Callstack()
		{
			SymCleanup(GetCurrentProcess());
		}
	};

	Str
	callstack_dump(Allocator allocator)
	{
		Str str = str_with_allocator(allocator);
		#if DEBUG
		static Debugger_Callstack _d;

		constexpr size_t MAX_NAME_LEN = 1024;
		constexpr size_t STACK_MAX = 4096;

		void* callstack[STACK_MAX];
		auto process_handle = GetCurrentProcess();

		//allocaet a buffer for the symbol info
		//windows lays the symbol info in memory in this form
		//[struct][name buffer]
		//and the name buffer size is the same as the MaxNameLen set below
		char buffer[sizeof(SYMBOL_INFO) + MAX_NAME_LEN];

		SYMBOL_INFO* symbol = (SYMBOL_INFO*)buffer;
		::memset(symbol, 0, sizeof(SYMBOL_INFO));

		symbol->MaxNameLen = MAX_NAME_LEN;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		size_t frames_count = CaptureStackBackTrace(0, STACK_MAX, callstack, NULL);
		for(size_t i = 0; i < frames_count; ++i)
		{
			if (SymFromAddr(process_handle, (DWORD64)(callstack[i]), NULL, symbol))
			{
				IMAGEHLP_LINE64 line;
				DWORD dis = 0;
				BOOL line_found = SymGetLineFromAddr64(process_handle, symbol->Address, &dis, &line);

				str_pushf(
					str,
					"[%zu]: %s, %s:%u\n",
					frames_count - i - 1,
					symbol->Name,
					line_found ? line.FileName : "<NO_FILE_FOUND>",
					line_found ? line.LineNumber + 1 : 0UL
				);
			}
			else
			{
				IMAGEHLP_LINE64 line;
				DWORD dis = 0;
				BOOL line_found = SymGetLineFromAddr64(process_handle, symbol->Address, &dis, &line);

				str_pushf(
					str,
					"[%zu]: unknown symbol, %s:%u\n",
					frames_count - i - 1,
					line_found ? line.FileName : "<NO_FILE_FOUND>",
					line_found ? line.LineNumber + 1 : 0UL
				);
			}
		}
		#endif
		return str;
	}
}
