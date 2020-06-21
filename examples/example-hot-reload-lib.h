#pragma once

#include "example-hot-reload-lib-exports.h"

namespace hot_reload_lib
{
	struct Foo
	{
		int x;
	};

	EXAMPLE_HOT_RELOAD_LIB_EXPORT Foo*
	foo_new();

	EXAMPLE_HOT_RELOAD_LIB_EXPORT void
	foo_free(Foo* self);
}

#define HOT_RELOAD_LIB_NAME "hot_reload_lib"

extern "C" EXAMPLE_HOT_RELOAD_LIB_EXPORT void*
rad_api(void* old_api, bool reload);
