#pragma once

#include <mn/Context.h>
#include <mn/Memory.h>

namespace mn
{
	struct Scope
	{
		memory::Arena* old_tmp;
		memory::Arena* tmp;

		Scope()
		{
			auto ctx = context_local();
			old_tmp = ctx->_allocator_tmp;
			tmp = allocator_arena_new();
			ctx->_allocator_tmp = tmp;
		}

		Scope(const Scope&) = delete;

		Scope(Scope&&) = delete;

		Scope& operator=(const Scope&) = delete;

		Scope& operator=(Scope&&) = delete;

		~Scope()
		{
			context_local()->_allocator_tmp = old_tmp;
			allocator_free(tmp);
		}
	};

	#define mn_SCOPE_1(x, y) x##y
	#define mn_SCOPE_2(x, y) mn_SCOPE_1(x, y)
	#define mn_SCOPE_3(x)    mn_SCOPE_2(x, __COUNTER__)
	#define mn_scope()   mn::Scope mn_SCOPE_3(_mn_scope_)
}
