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

	MN_EXPORT 
	template<typename ... TArgs> void
	log(const char* message, TArgs&& ... args);

	MN_EXPORT void
	log_stream_change(const Stream& stream);
}