#include "mn/Thread.h"

#if OS_LINUX

#include "mn/Pool.h"
#include "mn/Memory.h"

#include <pthread.h>

#include <assert.h>

namespace mn
{
	//for gcc linking bug
	Typed_Pool<pthread_mutex_t>*
	_mutex_pool()
	{
		thread_local Typed_Pool<pthread_mutex_t> _pool(1024, clib_allocator);
		return &_pool;
	}

	pthread_mutex_t*
	_allocators_mutex_new()
	{
		static pthread_mutex_t mtx;
		int result = pthread_mutex_init(&mtx, NULL);
		assert(result == 0);
		return &mtx;
	}


	Mutex
	_allocators_mutex()
	{
		static Mutex mtx = (Mutex)_allocators_mutex_new();
		return mtx;
	}

	Mutex
	mutex_new()
	{
		pthread_mutex_t* self = _mutex_pool()->get();
		int result = pthread_mutex_init(self, NULL);
		assert(result == 0);
		return (Mutex)self;
	}

	void
	mutex_lock(Mutex mutex)
	{
		pthread_mutex_t* self = (pthread_mutex_t*)mutex;
		int result = pthread_mutex_lock(self);
		assert(result == 0);
	}

	void
	mutex_unlock(Mutex mutex)
	{
		pthread_mutex_t* self = (pthread_mutex_t*)mutex;
		int result = pthread_mutex_unlock(self);
		assert(result == 0);
	}

	void
	mutex_free(Mutex mutex)
	{
		pthread_mutex_t* self = (pthread_mutex_t*)mutex;
		int result = pthread_mutex_destroy(self);
		assert(result == 0);
		_mutex_pool()->put(self);
	}
}

#endif