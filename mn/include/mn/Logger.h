#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/Thread.h"

namespace mn
{
	MN_EXPORT void
	log(Str str);

	MN_EXPORT void
	logger_stream_change(Stream& stream);

	MN_EXPORT Stream
	logger_stream_get();

	template<typename ... TArgs>
	inline static void
	logf(const char* message, TArgs&& ... args)
	{
		Str log_message = strf(message, std::forward<TArgs>(args)...);
		log(log_message);
	}

	inline static void 
	log(const char* message)
	{
		log(str_lit(message));
	}
}