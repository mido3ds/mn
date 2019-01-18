#include "mn/Thread.h"

#if OS_WINDOWS

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
		HANDLE handle;
	};

	Mutex
	_allocators_mutex()
	{
		static IMutex mtx{ "allocators mutex", CreateMutex(NULL,FALSE, L"allocators mutex") };
		return (Mutex)&mtx;
	}

	Mutex
	mutex_new(const char* name)
	{
		IMutex* self = alloc<IMutex>();
		self->name = name;
		self->handle = CreateMutexA(NULL, FALSE, name);
		assert(self->handle != INVALID_HANDLE_VALUE);
		return (Mutex)self;
	}

	void
	mutex_lock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		DWORD result = WaitForSingleObject(self->handle, INFINITE);
		assert(result == WAIT_OBJECT_0);
	}

	void
	mutex_unlock(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		BOOL result = ReleaseMutex(self->handle);
		assert(result == TRUE);
	}

	void
	mutex_free(Mutex mutex)
	{
		IMutex* self = (IMutex*)mutex;
		BOOL result = CloseHandle(self->handle);
		assert(result == TRUE);
		free(self);
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

	DWORD
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
}

#endif
