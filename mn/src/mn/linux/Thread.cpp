#include "mn/Thread.h"
#include "mn/Pool.h"
#include "mn/Memory.h"

#include <pthread.h>
#include <unistd.h>

#include <assert.h>

namespace mn
{
	pthread_mutex_t*
	_leak_allocator_mutex_new()
	{
		static pthread_mutex_t mtx;
		[[maybe_unused]] int result = pthread_mutex_init(&mtx, NULL);
		assert(result == 0);
		return &mtx;
	}


	Mutex
	_leak_allocator_mutex()
	{
		static Mutex mtx = (Mutex)_leak_allocator_mutex_new();
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
		Mutex self = alloc<IMutex>();
		self->name = name;
		[[maybe_unused]] int result = pthread_mutex_init(&self->handle, NULL);
		assert(result == 0);
		return self;
	}

	void
	mutex_lock(Mutex self)
	{
		[[maybe_unused]] int result = pthread_mutex_lock(&self->handle);
		assert(result == 0);
	}

	void
	mutex_unlock(Mutex self)
	{
		[[maybe_unused]] int result = pthread_mutex_unlock(&self->handle);
		assert(result == 0);
	}

	void
	mutex_free(Mutex self)
	{
		[[maybe_unused]] int result = pthread_mutex_destroy(&self->handle);
		assert(result == 0);
		free(self);
	}


	//Mutex_RW API
	struct IMutex_RW
	{
		pthread_rwlock_t lock;
		const char* name;
	};

	Mutex_RW
	mutex_rw_new(const char* name)
	{
		Mutex_RW self = alloc<IMutex_RW>();
		pthread_rwlock_init(&self->lock, NULL);
		self->name = name;
		return self;
	}

	void
	mutex_rw_free(Mutex_RW self)
	{
		pthread_rwlock_destroy(&self->lock);
		free(self);
	}

	void
	mutex_read_lock(Mutex_RW self)
	{
		pthread_rwlock_rdlock(&self->lock);
	}

	void
	mutex_read_unlock(Mutex_RW self)
	{
		pthread_rwlock_unlock(&self->lock);
	}

	void
	mutex_write_lock(Mutex_RW self)
	{
		pthread_rwlock_wrlock(&self->lock);
	}

	void
	mutex_write_unlock(Mutex_RW self)
	{
		pthread_rwlock_unlock(&self->lock);
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
		Thread self = (Thread)user_data;
		if(self->func)
			self->func(self->user_data);
		return 0;
	}

	Thread
	thread_new(Thread_Func func, void* arg, const char* name)
	{
		Thread self = alloc<IThread>();
		self->func = func;
		self->user_data = arg;
		self->name = name;
		[[maybe_unused]] int result = pthread_create(&self->handle, NULL, _thread_start, self);
		assert(result == 0 && "pthread_create failed");
		return self;
	}

	void
	thread_free(Thread self)
	{
		free(self);
	}

	void
	thread_join(Thread self)
	{
		[[maybe_unused]] int result = pthread_join(self->handle, NULL);
		assert(result == 0 && "pthread_join failed");
	}

	void
	thread_sleep(uint32_t milliseconds)
	{
		usleep(milliseconds * 1000);
	}
}
