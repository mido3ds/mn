#include "mn/Thread.h"

#if OS_WINDOWS

#include <assert.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace mn
{
	Mutex
	_allocators_mutex()
	{
		static Mutex mtx = (Mutex)CreateMutex(NULL, FALSE, NULL);
		return mtx;
	}

	Mutex
	mutex_new()
	{
		HANDLE handle = CreateMutex(NULL, FALSE, NULL);
		assert(handle != INVALID_HANDLE_VALUE);
		return (Mutex)handle;
	}

	void
	mutex_lock(Mutex mutex)
	{
		HANDLE handle = (HANDLE)mutex;
		DWORD result = WaitForSingleObject(handle, INFINITE);
		assert(result == WAIT_OBJECT_0);
	}

	void
	mutex_unlock(Mutex mutex)
	{
		HANDLE handle = (HANDLE)mutex;
		BOOL result = ReleaseMutex(handle);
		assert(result == TRUE);
	}

	void
	mutex_free(Mutex mutex)
	{
		HANDLE handle = (HANDLE)mutex;
		BOOL result = CloseHandle(handle);
		assert(result == TRUE);
	}
}

#endif
