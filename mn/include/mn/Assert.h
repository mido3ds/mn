#pragma once

#ifdef NDEBUG
	#define mn_assert(condition, description) ((void)0)
#else
	#include "mn/Exports.h"
	namespace mn
	{
		[[noreturn]] MN_EXPORT void
		_panic(const char* cause);
	}

    #define mn_assert(condition, description) (void)(                                        \
            (!!(condition)) ||                                                               \
            (mn::_panic("Assertion failed, (" #condition ") is false, " description), 0) \
        )
#endif
