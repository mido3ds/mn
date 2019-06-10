#include "mn/Logger.h"
#include "mn/IO.h"

namespace mn
{

	Logger* logger_get_instance()
	{
		static Logger _log = Logger{ stream_stderr(), mutex_new("log mutex") };
		return &_log;
	}

	void logger_free()
	{
		stream_free(logger_get_instance()->current_stream);
		mutex_free(logger_get_instance()->mtx);
	}

	void logger_stream_change(Stream& stream)
	{
		mutex_lock(logger_get_instance()->mtx);

		logger_get_instance()->current_stream = stream;

		mutex_unlock(logger_get_instance()->mtx);
	}

	Stream logger_stream_get()
	{
		return logger_get_instance()->current_stream;
	}
}