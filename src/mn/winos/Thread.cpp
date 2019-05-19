#include "mn/Thread.h"
#include "mn/Memory.h"

#include <assert.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace mn
{
	struct IMutex
	{
		const char* name;
		CRITICAL_SECTION cs;
	};

	struct Leak_Allocator_Mutex
	{
		IMutex self;

		Leak_Allocator_Mutex()
		{
			self.name = "allocators mutex";
			InitializeCriticalSectionAndSpinCount(&self.cs, 1<<14);
		}

		~Leak_Allocator_Mutex()
		{
			DeleteCriticalSection(&self.cs);
		}
	};

	Mutex
	_leak_allocator_mutex()
	{
		static Leak_Allocator_Mutex mtx;
		return (Mutex)&mtx.self;
	}

	Mutex
	mutex_new(const char* name)
	{
		IMutex* self = alloc<IMutex>();
		self->name = name;
		InitializeCriticalSectionAndSpinCount(&self->cs, 1<<14);
		return (Mutex)self;
	}

	void
	mutex_lock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		EnterCriticalSection(&self->cs);
	}

	void
	mutex_unlock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		LeaveCriticalSection(&self->cs);
	}

	void
	mutex_free(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		DeleteCriticalSection(&self->cs);
		free(self);
	}


	//Mutex_RW API
	struct IMutex_RW
	{
		SRWLOCK lock;
		const char* name;
	};

	Mutex_RW
	mutex_rw_new(const char* name)
	{
		IMutex_RW* self = alloc<IMutex_RW>();
		self->lock = SRWLOCK_INIT;
		self->name = name;
		return (Mutex_RW)self;
	}

	void
	mutex_rw_free(Mutex_RW mutex)
	{
		IMutex_RW* self = (IMutex_RW*)mutex;
		free(self);
	}

	void
	mutex_read_lock(Mutex_RW mutex)
	{
		IMutex_RW* self = (IMutex_RW*)mutex;
		AcquireSRWLockShared(&self->lock);
	}

	void
	mutex_read_unlock(Mutex_RW mutex)
	{
		IMutex_RW* self = (IMutex_RW*)mutex;
		ReleaseSRWLockShared(&self->lock);
	}

	void
	mutex_write_lock(Mutex_RW mutex)
	{
		IMutex_RW* self = (IMutex_RW*)mutex;
		AcquireSRWLockExclusive(&self->lock);
	}

	void
	mutex_write_unlock(Mutex_RW mutex)
	{
		IMutex_RW* self = (IMutex_RW*)mutex;
		ReleaseSRWLockExclusive(&self->lock);
	}


	//Thread API
	struct IThread
	{
		HANDLE handle;
		DWORD id;
		Thread_Func func;
		void* user_data;
		const char* name;
	};

	DWORD WINAPI
	_thread_start(LPVOID user_data)
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

		self->handle = CreateThread(NULL, //default security attributes
									0, //default stack size
									_thread_start, //thread start function
									self, //thread start function arg
									0, //default creation flags
									&self->id); //thread id
		return (Thread)self;
	}

	void
	thread_free(Thread thread)
	{
		IThread* self = (IThread*)thread;
		if(self->handle)
		{
			BOOL result = CloseHandle(self->handle);
			assert(result == TRUE);
		}
		free(self);
	}

	void
	thread_join(Thread thread)
	{
		IThread* self = (IThread*)thread;
		if(self->handle)
		{
			DWORD result = WaitForSingleObject(self->handle, INFINITE);
			assert(result == WAIT_OBJECT_0);
		}
	}

	void
	thread_sleep(uint32_t milliseconds)
	{
		Sleep(DWORD(milliseconds));
	}


	//Limbo
	struct ILimbo
	{
		CRITICAL_SECTION cs;
		CONDITION_VARIABLE cv;
		const char* name;
	};

	Limbo
	limbo_new(const char* name)
	{
		ILimbo* self = alloc<ILimbo>();
		InitializeCriticalSectionAndSpinCount(&self->cs, 4096);
		self->cv = CONDITION_VARIABLE_INIT;
		self->name = name;
		return (Limbo)self;
	}

	void
	limbo_free(Limbo limbo)
	{
		ILimbo* self = (ILimbo*)limbo;
		DeleteCriticalSection(&self->cs);
		free(self);
	}

	void
	limbo_lock(Limbo limbo, Limbo_Predicate* pred)
	{
		ILimbo* self = (ILimbo*)limbo;

		EnterCriticalSection(&self->cs);

		while(pred->should_wake() == false)
			SleepConditionVariableCS(&self->cv, &self->cs, INFINITE);
	}

	void
	limbo_unlock_one(Limbo limbo)
	{
		ILimbo* self = (ILimbo*)limbo;

		LeaveCriticalSection(&self->cs);
		WakeConditionVariable(&self->cv);
	}

	void
	limbo_unlock_all(Limbo limbo)
	{
		ILimbo* self = (ILimbo*)limbo;

		LeaveCriticalSection(&self->cs);
		WakeAllConditionVariable(&self->cv);
	}
}
