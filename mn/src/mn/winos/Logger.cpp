#include "mn/Logger.h"
#include "mn/IO.h"

namespace mn
{
	static Logger logger;

	template<typename ... TArgs>
	void log(const char* message, TArgs&& ... args)
	{
		mutex_lock(logger.mtx);

		vprintf(logger.current_stream, message, std::forward<TArgs>(args)...);

		mutex_unlock(logger.mtx);
	}
 
	void log_stream_change(Stream& stream)
	{
		mutex_lock(logger.mtx);

		logger.current_stream = stream;

		mutex_unlock(logger.mtx);
	}
}