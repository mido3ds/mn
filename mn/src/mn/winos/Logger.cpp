#include "mn/Logger.h"
#include "mn/IO.h"

namespace mn
{
	static Logger logger;

	void log_stream_change(Stream& stream)
	{
		mutex_lock(logger.mtx);

		logger.current_stream = stream;

		mutex_unlock(logger.mtx);
	}
}