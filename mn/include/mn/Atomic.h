#pragma once

#include "mn/Exports.h"

#include <stdint.h>

namespace mn
{
	//increments the variable and returns the value AFTER inc
	MN_EXPORT int32_t
	atomic_inc(int32_t &self);

	MN_EXPORT int64_t
	atomic_inc(int64_t &self);

	//adds value to the variable and returns the value BEFORE add
	MN_EXPORT int32_t
	atomic_add(int32_t &self, int32_t value);

	MN_EXPORT int64_t
	atomic_add(int64_t &self, int64_t value);
}