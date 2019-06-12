#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/IO.h"

namespace mn
{
	MN_EXPORT void
	log(Str str);

	MN_EXPORT Stream
	log_stream_set(Stream stream);

	MN_EXPORT Stream
	log_stream();

	MN_EXPORT void
	logger_free();

	inline static void
	logf(const char* message)
	{
		log(str_lit(message));
	}

	template<typename ... TArgs>
	inline static void
	logf(const char* message, TArgs&& ... args)
	{
		Str log_message = strf(message, std::forward<TArgs>(args)...);
		log(log_message);
		str_free(log_message);
	}
}