#pragma once

#include "mn/Exports.h"

#include <stdint.h>

namespace mn
{
	// a process id
	struct Process
	{
		uint64_t id;
	};

	// returns the current process id
	MN_EXPORT Process
	process_id();

	// returns the parent id of this process, if it has no parent it will
	// return a zero handle Process{}
	MN_EXPORT Process
	process_parent_id();

	// tries to kill the given process and returns whether it was successful
	MN_EXPORT bool
	process_kill(Process p);

	// returns whether the given process is alive
	MN_EXPORT bool
	process_alive(Process p);
}
