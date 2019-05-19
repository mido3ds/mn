#include "mn/OS.h"
#include "mn/Debug.h"

#include <stdio.h>
#include <stdlib.h>

namespace mn
{
	void
	_panic(const char* cause)
	{
		::fprintf(stderr, "[PANIC]: %s\n%s\n", cause, callstack_dump().ptr);
		::exit(-1);
	}
}