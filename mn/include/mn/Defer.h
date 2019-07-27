#pragma once

namespace mn
{
	template <typename F>
	struct Defer {
		F f;
		Defer(F f) : f(f) {}
		~Defer() { f(); }
	};

	template<typename F>
	inline static Defer<F>
	make_defer(F f)
	{
		return Defer<F>(f);
	}

	#define mn_DEFER_1(x, y) x##y
	#define mn_DEFER_2(x, y) mn_DEFER_1(x, y)
	#define mn_DEFER_3(x)    mn_DEFER_2(x, __COUNTER__)
	#define mn_defer(code)   auto mn_DEFER_3(_defer_) = mn::make_defer([&](){code;})
}