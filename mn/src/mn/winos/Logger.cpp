#include "mn/Logger.h"
#include "mn/Thread.h"
#include "mn/File.h"

namespace mn
{
	struct Logger
	{
		Logger()
		{
			current_stream = file_stderr();
			mtx = mutex_new("log mutex");
		}

		~Logger()
		{
			mutex_free(mtx);
		}

		Stream current_stream;
		Mutex mtx;
	};

	inline static Logger*
	logger_instance()
	{
		static Logger _log;
		return &_log;
	}
	
	Stream
	log_stream_set(Stream stream)
	{
		Logger* _log = logger_instance();

		mutex_lock(_log->mtx);

		Stream old_stream = _log->current_stream;

		_log->current_stream = stream;

		mutex_unlock(_log->mtx);

		return old_stream;
	}

	Stream
	log_stream()
	{
		return logger_instance()->current_stream;
	}

	void
	log(Str str)
	{
		Logger* _log = logger_instance();

		mutex_lock(_log->mtx);

		stream_print(_log->current_stream, str.ptr);

		mutex_unlock(_log->mtx);
	}
}