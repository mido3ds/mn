#include "mn/Logger.h"
#include "mn/IO.h"

namespace mn
{
	struct Logger
	{
		Stream current_stream;
		Mutex mtx;
	};

	static Logger _log = Logger{ stream_stderr(), mutex_new("log mutex") };

	void logger_free()
	{
		stream_free(_log.current_stream);
		mutex_free(_log.mtx);
	}

	void logger_stream_change(Stream& stream)
	{
		mutex_lock(_log.mtx);

		_log.current_stream = stream;

		mutex_unlock(_log.mtx);
	}

	Stream logger_stream_get()
	{
		return _log.current_stream;
	}

	void log(Str str)
	{
		mutex_lock(_log.mtx);
		vprintf(logger_stream_get(), str.ptr);
		mutex_unlock(_log.mtx);
	}
}