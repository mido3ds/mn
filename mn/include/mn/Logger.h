#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/Thread.h"

namespace mn
{
	struct Logger;

	template<typename ... TArgs>
	inline static size_t
	log(const char* message, TArgs&& ... args)
	{
		mutex_lock(get_logger_instance()->mtx);

		vprintf(log_stream_get(), message, std::forward<TArgs>(args)...);

		mutex_unlock(get_logger_instance()->mtx);
	}

	MN_EXPORT Logger*
	get_logger_instance();

	MN_EXPORT void
	logger_free();

	MN_EXPORT void
	log_stream_change(Stream& stream);

	MN_EXPORT Stream
	log_stream_get();
}