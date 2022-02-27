#pragma once

#include "mn/Str.h"
#include "mn/Fmt.h"
#include "mn/Assert.h"

namespace mn
{
	// an error message (this type uses RAII to manage its memory)
	struct Err
	{
		Str msg;

		// creates a new empty error (not an error)
		Err(): msg(str_new()) {}

		// creates a new error with the given error message
		template<typename... TArgs>
		Err(const char* fmt, TArgs&& ... args)
			:msg(strf(fmt, std::forward<TArgs>(args)...))
		{}

		Err(const Err& other)
			:msg(clone(other.msg))
		{}

		Err(Err&& other)
			:msg(other.msg)
		{
			other.msg = str_new();
		}

		~Err() { str_free(msg); }

		Err&
		operator=(const Err& other)
		{
			str_clear(msg);
			str_push(msg, other.msg);
			return *this;
		}

		Err&
		operator=(Err&& other)
		{
			str_free(msg);
			msg = other.msg;
			other.msg = str_new();
			return *this;
		}

		// casts the given error to a boolean, (false = no error, true = error exists)
		explicit operator bool() const { return msg.count != 0; }
		bool operator==(bool v) const { return (msg.count != 0) == v; }
		bool operator!=(bool v) const { return !operator==(v); }
	};

	// represents a result of a function which can fail, it holds either the value or the error
	// error types shouldn't be of the same type as value types
	template<typename T, typename E = Err>
	struct Result
	{
		static_assert(!std::is_same_v<Err, T>, "Error can't be of the same type as value");

		T val;
		E err;

		// creates a result instance from an error
		Result(E e)
			:err(e)
		{
			::memset(&val, 0, sizeof(val));
		}

		// creates a result instance from a value
		template<typename... TArgs>
		Result(TArgs&& ... args)
			:val(std::forward<TArgs>(args)...),
			 err(E{})
		{}

		Result(const Result&) = delete;

		Result(Result&&) = default;

		Result& operator=(const Result&) = delete;

		Result& operator=(Result&&) = default;

		~Result() = default;
	};

	// destruct overload for result
	template<typename T, typename TErr>
	inline static void
	destruct(Result<T, TErr>& self)
	{
		if (self.err)
			destruct(self.err);
		else
			destruct(self.val);
	}
}

namespace fmt
{
	template<>
	struct formatter<mn::Err>
	{
		template<typename ParseContext>
		constexpr auto
		parse(ParseContext &ctx)
		{
			return ctx.begin();
		}

		template<typename FormatContext>
		auto
		format(const mn::Err &err, FormatContext &ctx)
		{
			return format_to(ctx.out(), "{}", err.msg);
		}
	};
} // namespace fmt
