#include "mn/Logger.h"
#include "mn/IO.h"

namespace mn
{
	struct Logger
	{
		Stream current_stream;
		Mutex mtx;
	};

	Logger* get_logger_instance()
	{
		static Logger _log = Logger{ stream_stderr(), mutex_new("log mutex") };
		return &_log;
	}

	void logger_free()
	{
		stream_free(get_logger_instance()->current_stream);
		mutex_free(get_logger_instance()->mtx);
	}

	void log_stream_change(Stream& stream)
	{
		mutex_lock(get_logger_instance()->mtx);

		get_logger_instance()->current_stream = stream;

		mutex_unlock(get_logger_instance()->mtx);
	}

	Stream log_stream_get()
	{
		return get_logger_instance()->current_stream;
	}
}