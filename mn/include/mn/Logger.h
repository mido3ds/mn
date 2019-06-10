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
	inline static size_t
	log(const char* message, TArgs&& ... args)
	{
		mutex_lock(logger.mtx);

		vprintf(logger.current_stream, message, std::forward<TArgs>(args)...);

		mutex_unlock(logger.mtx);
	}

	MN_EXPORT void
	log_stream_change(const Stream& stream);
}