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

	//increments the variable and returns the value AFTER dec
	MN_EXPORT int32_t
	atomic_dec(int32_t &self);

	MN_EXPORT int64_t
	atomic_dec(int64_t &self);

	//adds value to the variable and returns the value BEFORE add
	MN_EXPORT int32_t
	atomic_add(int32_t &self, int32_t value);

	MN_EXPORT int64_t
	atomic_add(int64_t &self, int64_t value);

	//returns the value BEFORE the exchange
	MN_EXPORT int32_t
	atomic_exchange(int32_t &self, int32_t value);

	MN_EXPORT int64_t
	atomic_exchange(int64_t &self, int64_t value);

	MN_EXPORT int32_t
	atomic_load(int32_t &self);

	MN_EXPORT int64_t
	atomic_load(int64_t &self);
}