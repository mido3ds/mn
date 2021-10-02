#include "mn/Assert.h"
#include "mn/Log.h"

namespace mn
{
	void
	_report_assert_message(const char* expr, const char* message, const char* file, int line)
	{
		auto msg = mn::str_tmpf("Assertion Failure: {}, message: {}, in file: {}, line: {}", expr, message, file, line);
		_log_critical_str(msg.ptr);
	}
}