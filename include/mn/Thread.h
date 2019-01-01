#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

namespace mn
{
	/**
	 * Mutex Handle
	 */
	MS_HANDLE(Mutex);

	API_MN Mutex
	_allocators_mutex();

	/**
	 * @brief      Creates a new mutex
	 */
	API_MN Mutex
	mutex_new();

	/**
	 * @brief      Locks the given mutex
	 *
	 * @param[in]  mutex  The mutex
	 */
	API_MN void
	mutex_lock(Mutex mutex);

	/**
	 * @brief      Unlocks the given mutex
	 *
	 * @param[in]  mutex  The mutex
	 */
	API_MN void
	mutex_unlock(Mutex mutex);

	/**
	 * @brief      Frees the given mutex
	 *
	 * @param[in]  mutex  The mutex
	 */
	API_MN void
	mutex_free(Mutex mutex);

	/**
	 * @brief      Destruct function overload for the mutex type
	 *
	 * @param[in]  mutex  The mutex
	 */
	inline static void
	destruct(Mutex mutex)
	{
		mutex_free(mutex);
	}
}
