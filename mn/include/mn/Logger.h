#pragma once

#include "mn/Exports.h"
#include "mn/Stream.h"
#include "mn/Thread.h"

namespace mn
{
	struct Logger
	{
		Stream current_stream;
		Mutex mtx;
	};

	template<typename ... TArgs>
	inline static void
	log(const char* message, TArgs&& ... args)
	{
		mutex_lock(logger_get_instance()->mtx);

		vprintf(logger_stream_get(), message, std::forward<TArgs>(args)...);

		mutex_unlock(logger_get_instance()->mtx);
	}

	MN_EXPORT Logger*
	logger_get_instance();

	MN_EXPORT void
	logger_free();

	MN_EXPORT void
	logger_stream_change(Stream& stream);

	MN_EXPORT Stream
	logger_stream_get();
}