#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"

#include <stdint.h>

namespace mn
{
	/**
	 * Mutex Handle
	 */
	MS_HANDLE(Mutex);

	API_MN Mutex
	_leak_allocator_mutex();

	/**
	 * @brief      Creates a new mutex
	 *
	 * @param[in]  name  The mutex name
	 */
	API_MN Mutex
	mutex_new(const char* name = "Mutex");

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


	//Read preferring multi-reader single-writer mutex
	MS_HANDLE(Mutex_RW);

	/**
	 * @brief      Creates a new multi-reader single-writer mutex
	 *
	 * @param[in]  name  The mutex name
	 */
	API_MN Mutex_RW
	mutex_rw_new(const char* name = "Mutex_RW");

	/**
	 * @brief      Frees the mutex
	 */
	API_MN void
	mutex_rw_free(Mutex_RW mutex);

	/**
	 * @brief      destruct overload function for Mutex_RW
	 */
	inline static void
	destruct(Mutex_RW mutex)
	{
		mutex_rw_free(mutex);
	}

	/**
	 * @brief      Locks the mutex for a read operation
	 */
	API_MN void
	mutex_read_lock(Mutex_RW mutex);

	/**
	 * @brief      Unlocks the mutex from a read operation
	 */
	API_MN void
	mutex_read_unlock(Mutex_RW mutex);

	/**
	 * @brief      Locks the mutex for a write operation
	 */
	API_MN void
	mutex_write_lock(Mutex_RW mutex);

	/**
	 * @brief      Unlocks the mutex from a write operation
	 */
	API_MN void
	mutex_write_unlock(Mutex_RW mutex);


	//Thread API
	MS_HANDLE(Thread);

	/**
	 * Thread function
	 */
	using Thread_Func = void(*)(void*);

	/**
	 * @brief      Creates a new thread
	 *
	 * @param[in]  func  The function to run
	 * @param      arg   The argument to pass to the function
	 * @param[in]  name  The name of the thread
	 */
	API_MN Thread
	thread_new(Thread_Func func, void* arg, const char* name = "Thread");

	/**
	 * @brief      Frees the thread
	 */
	API_MN void
	thread_free(Thread thread);

	/**
	 * @brief      destruct overload for the thread type
	 */
	inline static void
	destruct(Thread thread)
	{
		thread_free(thread);
	}

	/**
	 * @brief      joins the execution of the calling thread with the given thread
	 */
	API_MN void
	thread_join(Thread thread);

	/**
	 * @brief      sleeps the calling thread by the given milliseconds
	 */
	API_MN void
	thread_sleep(uint32_t milliseconds);


	//Condition Variable + Mutex Combo
	struct Limbo_Predicate
	{
		virtual bool should_wake() = 0;
	};

	MS_HANDLE(Limbo);

	API_MN Limbo
	limbo_new(const char* name);

	API_MN void
	limbo_free(Limbo limbo);

	//wait for the predicate to be true
	API_MN void
	limbo_lock(Limbo limbo, Limbo_Predicate* pred);

	//leaves the limbo + notify one combo
	API_MN void
	limbo_unlock_one(Limbo limbo);

	//leaves the limbo + notify all combo
	API_MN void
	limbo_unlock_all(Limbo limbo);

	template<typename T>
	struct Limbo_Lambda final: Limbo_Predicate
	{
		T predicate;

		Limbo_Lambda(T&& pred)
			:predicate(pred)
		{}

		bool should_wake() override { return predicate(); }
	};

	template<typename TPredicate>
	inline static void
	limbo_lock(Limbo limbo, TPredicate&& pred)
	{
		Limbo_Lambda lambda(pred);
		limbo_lock(limbo, &lambda);
	}
}
