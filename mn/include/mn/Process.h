#pragma once

#include "mn/Exports.h"

#include <stdint.h>

namespace mn
{
	struct Process
	{
		uint64_t id;
	};

	// process_current returns the current process id
	MN_EXPORT Process
	process_id();

	// process_parent_id returns the parent id of this process, if it has no parent it will
	// return a zero handle Process{}
	MN_EXPORT Process
	process_parent_id();

	// process_kill tries to kill the given process and returns whether it was successful
	MN_EXPORT bool
	process_kill(Process p);

	// process_alive returns whether the given process is alive
	MN_EXPORT bool
	process_alive(Process p);
}
