#include "mn/Thread.h"

#if OS_LINUX

#include "mn/Pool.h"
#include "mn/Memory.h"

#include <pthread.h>
#include <unistd.h>

#include <assert.h>

//to supress warnings in release mode
#define UNUSED(x) ((void)(x))

namespace mn
{
	pthread_mutex_t*
	_allocators_mutex_new()
	{
		static pthread_mutex_t mtx;
		int result = pthread_mutex_init(&mtx, NULL);
		assert(result == 0);
		UNUSED(result);
		return &mtx;
	}


	Mutex
	_allocators_mutex()
	{
		static Mutex mtx = (Mutex)_allocators_mutex_new();
		return mtx;
	}


	struct IMutex
	{
		pthread_mutex_t handle;
		const char* name;
	};

	Mutex
	mutex_new(const char* name)
	{
		IMutex* self = alloc<IMutex>();
		self->name = name;
		int result = pthread_mutex_init(&self->handle, NULL);
		assert(result == 0);
		UNUSED(result);
		return (Mutex)self;
	}

	void
	mutex_lock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		int result = pthread_mutex_lock(&self->handle);
		assert(result == 0);
		UNUSED(result);
	}

	void
	mutex_unlock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		int result = pthread_mutex_unlock(&self->handle);
		assert(result == 0);
		UNUSED(result);
	}

	void
	mutex_free(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		int result = pthread_mutex_destroy(&self->handle);
		assert(result == 0);
		UNUSED(result);
		free(self);
	}


	//Thread
	struct IThread
	{
		pthread_t handle;
		Thread_Func func;
		void* user_data;
		const char* name;
	};

	void*
	_thread_start(void* user_data)
	{
		IThread* self = (IThread*)user_data;
		if(self->func)
			self->func(self->user_data);
		return 0;
	}

	Thread
	thread_new(Thread_Func func, void* arg, const char* name)
	{
		IThread* self = alloc<IThread>();
		self->func = func;
		self->user_data = arg;
		self->name = name;
		int result = pthread_create(&self->handle, NULL, _thread_start, self);
		assert(result == 0 && "pthread_create failed");
		UNUSED(result);
		return (Thread)self;
	}

	void
	thread_free(Thread thread)
	{
		IThread* self = (IThread*)thread;
		free(self);
	}

	void
	thread_join(Thread thread)
	{
		IThread* self = (IThread*)thread;
		int result = pthread_join(self->handle, NULL);
		assert(result == 0 && "pthread_join failed");
		UNUSED(result);
	}

	void
	thread_sleep(uint32_t milliseconds)
	{
		usleep(milliseconds * 1000);
	}
}

#endif