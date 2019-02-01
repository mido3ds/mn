#include "mn/File.h"

#if OS_WINDOWS

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mbstring.h>
#include <tchar.h>

#include "mn/Thread.h"
#include "mn/OS.h"

namespace mn
{
	Block
	to_os_encoding(const Str& utf8)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8,
			MB_PRECOMPOSED, utf8.ptr, utf8.count, NULL, 0);

		//+1 for the null termination
		size_t required_size = (size_needed + 1) * sizeof(WCHAR);
		Block buffer = alloc_from(allocator_tmp(), required_size, alignof(WCHAR));

		size_needed = MultiByteToWideChar(CP_UTF8,
			MB_PRECOMPOSED, utf8.ptr, utf8.cap, (LPWSTR)buffer.ptr, buffer.size);
		return buffer;
	}

	Block
	to_os_encoding(const char* utf8)
	{
		return to_os_encoding(str_lit(utf8));
	}

	Str
	_from_os_encoding(Block os_str, Allocator allocator)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, NULL, (LPWSTR)os_str.ptr,
			os_str.size / sizeof(WCHAR), NULL, 0, NULL, NULL);
		if (size_needed == 0)
			return str_with_allocator(allocator);

		Str buffer = str_with_allocator(allocator);
		buf_resize(buffer, size_needed);

		size_needed = WideCharToMultiByte(CP_UTF8, NULL, (LPWSTR)os_str.ptr,
			os_str.size / sizeof(WCHAR), buffer.ptr, buffer.count, NULL, NULL);
		--buffer.count;
		return buffer;
	}

	Str
	from_os_encoding(Block os_str)
	{
		return _from_os_encoding(os_str, allocator_tmp());
	}


	//File
	File
	_file_stdout()
	{
		constexpr const uint32_t MY_ENABLE_VIRTUAL_TERMINAL_PROCESSING = 4;

		File file;
		file.windows_handle = GetStdHandle(STD_OUTPUT_HANDLE);

		DWORD mode;
		GetConsoleMode(file.windows_handle, &mode);
		mode |= MY_ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(file.windows_handle, mode);
		return file;
	}

	struct STDOUT_Mutex_Wrapper
	{
		Mutex mtx;

		STDOUT_Mutex_Wrapper()
		{
			mtx = mutex_new();
		}

		~STDOUT_Mutex_Wrapper()
		{
			mutex_free(mtx);
		}
	};

	Mutex
	_mutex_stdout()
	{
		static STDOUT_Mutex_Wrapper _stdout_mtx;
		return _stdout_mtx.mtx;
	}

	File
	file_stdout()
	{
		static File _stdout = _file_stdout();
		return _stdout;
	}

	File
	_file_stderr()
	{
		File file;
		file.windows_handle = GetStdHandle(STD_ERROR_HANDLE);
		return file;
	}

	Mutex
	_mutex_stderr()
	{
		static Mutex _stdout_mtx = mutex_new();
		return _stdout_mtx;
	}

	File
	file_stderr()
	{
		static File _stderr = _file_stderr();
		return _stderr;
	}

	Mutex
	_mutex_stdin()
	{
		static Mutex _stdout_mtx = mutex_new();
		return _stdout_mtx;
	}

	File
	_file_stdin()
	{
		File file;
		file.windows_handle = GetStdHandle(STD_INPUT_HANDLE);
		return file;
	}

	File
	file_stdin()
	{
		static File _stdin = _file_stdin();
		return _stdin;
	}

	File
	file_open(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode)
	{
		File result{};

		//translate the io mode
		DWORD desired_access;
		switch(io_mode)
		{
			case IO_MODE::READ:
				desired_access = GENERIC_READ;
				break;

			case IO_MODE::WRITE:
				desired_access = GENERIC_WRITE;
				break;

			case IO_MODE::READ_WRITE:
			default:
				desired_access = GENERIC_READ | GENERIC_WRITE;
				break;
		}

		//translate the open mode
		DWORD creation_disposition;
		switch(open_mode)
		{
			case OPEN_MODE::CREATE_ONLY:
				creation_disposition = CREATE_NEW;
				break;

			case OPEN_MODE::OPEN_OVERWRITE:
				creation_disposition = TRUNCATE_EXISTING;
				break;

			case OPEN_MODE::OPEN_ONLY:
			case OPEN_MODE::OPEN_APPEND:
				creation_disposition = OPEN_EXISTING;
				break;

			case OPEN_MODE::CREATE_APPEND:
				creation_disposition = OPEN_ALWAYS;
				break;

			case OPEN_MODE::CREATE_OVERWRITE:
			default:
				creation_disposition = CREATE_ALWAYS;
				break;
		}

		auto os_str = to_os_encoding(filename);
		LPWSTR win_filename = (LPWSTR)os_str.ptr;
		result.windows_handle = CreateFile (win_filename, desired_access, 0, NULL,
											creation_disposition,
											FILE_ATTRIBUTE_NORMAL,
											NULL);

		if(result.windows_handle == INVALID_HANDLE_VALUE)
			return File{};

		if(open_mode == OPEN_MODE::CREATE_APPEND ||
		   open_mode == OPEN_MODE::OPEN_APPEND)
		{
			SetFilePointer (result.windows_handle,	//file handle
							NULL,					//distance to move low part
				 			NULL,					//ditance to mvoe high part
				 			FILE_END); 				//movement point of reference
		}
		return result;
	}

	bool
	file_close(File handle)
	{
		return CloseHandle(handle.windows_handle);
	}

	bool
	file_valid(File handle)
	{
		return handle.windows_handle != nullptr;
	}

	size_t
	file_write(File handle, Block data)
	{
		DWORD bytes_written = 0;

		Mutex mtx = nullptr;
		if(handle.windows_handle == file_stdout().windows_handle)
			mtx = _mutex_stdout();
		else if(handle.windows_handle == file_stderr().windows_handle)
			mtx = _mutex_stderr();

		if (mtx) mutex_lock(mtx);
			WriteFile(handle.windows_handle, data.ptr, data.size, &bytes_written, NULL);
		if (mtx) mutex_unlock(mtx);
		return bytes_written;
	}

	size_t
	file_read(File handle, Block data)
	{
		DWORD bytes_read = 0;
		Mutex mtx = nullptr;
		if(handle.windows_handle == file_stdin().windows_handle)
			mtx = _mutex_stdin();

		if(mtx) mutex_lock(mtx);
			ReadFile(handle.windows_handle, data.ptr, data.size, &bytes_read, NULL);
		if(mtx) mutex_unlock(mtx);
		return bytes_read;
	}

	int64_t
	file_size(File handle)
	{
		LARGE_INTEGER size;
		if(GetFileSizeEx(handle.windows_handle, &size))
		{
			return *(int64_t*)(&size);
		}
		return -1;
	}

	int64_t
	file_cursor_pos(File handle)
	{
		LARGE_INTEGER position, offset;
		offset.QuadPart = 0;
		if(SetFilePointerEx(handle.windows_handle, offset, &position, FILE_CURRENT))
		{
			return *(int64_t*)(&position);
		}
		return -1;
	}

	bool
	file_cursor_move(File handle, int64_t offset)
	{
		LARGE_INTEGER position, win_offset;
		win_offset.QuadPart = offset;
		return SetFilePointerEx(handle.windows_handle, win_offset, &position, FILE_CURRENT);
	}

	bool
	file_cursor_move_to_start(File handle)
	{
		LARGE_INTEGER position, offset;
		offset.QuadPart = 0;
		return SetFilePointerEx(handle.windows_handle, offset, &position, FILE_BEGIN);
	}

	bool
	file_cursor_move_to_end(File handle)
	{
		LARGE_INTEGER position, offset;
		offset.QuadPart = 0;
		return SetFilePointerEx(handle.windows_handle, offset, &position, FILE_END);
	}


	//File System api
	Str
	path_os_encoding(const char* path)
	{
		size_t str_len = ::strlen(path);
		Str res = str_with_allocator(allocator_tmp());
		buf_reserve(res, str_len + 1);

		for (size_t i = 0; i < str_len; ++i)
		{
			if (path[i] == '/')
				buf_push(res, '\\');
			else
				buf_push(res, path[i]);
		}

		str_null_terminate(res);
		return res;
	}

	Str&
	path_sanitize(Str& path)
	{
		char prev = '\0';
		char *it_write = path.ptr;
		const char *it_read = path.ptr;
		//skip all the /, \ on front
		while (it_read && *it_read != '\0' && (*it_read == '/' || *it_read == '\\'))
			it_read = rune_next(it_read);

		while (it_read && *it_read != '\0')
		{
			int c = rune_read(it_read);
			if (c == '\\' && prev != '\\')
			{
				*it_write = '/';
			}
			else if (c == '\\' && prev == '\\')
			{
				while (it_read && *it_read != '\0' && *it_read == '\\')
					it_read = rune_next(it_read);
				continue;
			}
			else if (c == '/' && prev == '/')
			{
				while (it_read && *it_read != '\0' && *it_read == '/')
					it_read = rune_next(it_read);
				continue;
			}
			else
			{
				size_t size = rune_size(c);
				char* c_it = (char*)&c;
				for (size_t i = 0; i < size; ++i)
					*it_write = *c_it;
			}
			prev = c;
			it_read = rune_next(it_read);
			it_write = (char*)rune_next(it_write);
		}
		path.count = it_write - path.ptr;
		if (prev == '\\' || prev == '/')
			--path.count;
		str_null_terminate(path);
		return path;
	}

	Str&
	path_normalize(Str& path)
	{
		for (char& c : path)
		{
			if (c == '\\')
				c = '/';
		}
		return path;
	}

	bool
	path_exists(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD attributes = GetFileAttributes((LPCWSTR)os_path.ptr);
		return attributes != INVALID_FILE_ATTRIBUTES;
	}

	bool
	path_is_folder(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD attributes = GetFileAttributes((LPCWSTR)os_path.ptr);
		return (attributes != INVALID_FILE_ATTRIBUTES &&
				attributes &  FILE_ATTRIBUTE_DIRECTORY);
	}

	bool
	path_is_file(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD attributes = GetFileAttributes((LPCWSTR)os_path.ptr);
		return (attributes != INVALID_FILE_ATTRIBUTES &&
				!(attributes &  FILE_ATTRIBUTE_DIRECTORY));
	}

	Str
	path_current(Allocator allocator)
	{
		DWORD required_size = GetCurrentDirectory(0, NULL);
		Block os_str = alloc_from(allocator_tmp(), required_size * sizeof(TCHAR), alignof(TCHAR));
		DWORD written_size = GetCurrentDirectory((DWORD)(os_str.size/sizeof(TCHAR)), (LPWSTR)os_str.ptr);
		assert((size_t)(written_size+1) == (os_str.size / sizeof(TCHAR)) && "GetCurrentDirectory Failed");
		Str res = _from_os_encoding(os_str, allocator);
		path_normalize(res);
		return res;
	}

	void
	path_current_change(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		bool result = SetCurrentDirectory((LPCWSTR)os_path.ptr);
		assert(result && "SetCurrentDirectory Failed");
	}

	Str
	path_absolute(const char* path, Allocator allocator)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD required_size = GetFullPathName((LPCWSTR)os_path.ptr, 0, NULL, NULL);
		Block full_path = alloc_from(allocator_tmp(), required_size * sizeof(TCHAR), alignof(TCHAR));
		DWORD written_size = GetFullPathName((LPCWSTR)os_path.ptr, required_size, (LPWSTR)full_path.ptr, NULL);
		assert(written_size+1 == required_size && "GetFullPathName failed");
		Str res = _from_os_encoding(full_path, allocator);
		path_normalize(res);
		return res;
	}

	Buf<Path_Entry>
	path_entries(const char* path, Allocator allocator)
	{
		//add the * at the end
		Str tmp_path = str_with_allocator(allocator_tmp());
		buf_reserve(tmp_path, ::strlen(path) + 3);
		str_push(tmp_path, path);
		if (tmp_path.count && tmp_path[tmp_path.count - 1] != '/')
			buf_push(tmp_path, '/');
		buf_push(tmp_path, '*');
		str_null_terminate(tmp_path);

		Buf<Path_Entry> res = buf_with_allocator<Path_Entry>(allocator);
		Block os_path = to_os_encoding(path_os_encoding(tmp_path));
		WIN32_FIND_DATA file_data{};
		HANDLE search_handle = FindFirstFileEx((LPCWSTR)os_path.ptr,
			FindExInfoBasic, &file_data, FindExSearchNameMatch, NULL, FIND_FIRST_EX_CASE_SENSITIVE);
		if (search_handle != INVALID_HANDLE_VALUE)
		{
			while (search_handle != INVALID_HANDLE_VALUE)
			{
				Path_Entry entry{};
				if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					entry.kind = Path_Entry::KIND_FOLDER;
				else
					entry.kind = Path_Entry::KIND_FILE;
				entry.name = _from_os_encoding(Block{
						(void*)file_data.cFileName,
						(_tcsclen(file_data.cFileName) + 1) * sizeof(TCHAR)
					}, allocator);
				path_normalize(entry.name);
				buf_push(res, entry);
				if (FindNextFile(search_handle, &file_data) == false)
					break;
			}
			bool result = FindClose(search_handle);
			assert(result && "FindClose failed");
		}
		return res;
	}

	//Tip 
	//Starting with Windows 10, version 1607, for the unicode version of this function (MoveFileW),
	//you can opt-in to remove the MAX_PATH limitation without prepending "\\?\". See the
	//"Maximum Path Length Limitation" section of Naming Files, Paths, and Namespaces for details.

	bool
	file_copy(const char* src, const char* dst)
	{
		//Str tmp = str_with_allocator(allocator_tmp());
		//str_pushf(tmp, "\\\\?\\%s", src);
		Block os_src = to_os_encoding(path_os_encoding(src));
		//str_clear(tmp);
		//str_pushf(tmp, "\\\\?\\%s", dst);
		Block os_dst = to_os_encoding(path_os_encoding(dst));
		return CopyFile((LPCWSTR)os_src.ptr, (LPCWSTR)os_dst.ptr, TRUE);
	}

	bool
	file_remove(const char* path)
	{
		//Str tmp = str_with_allocator(allocator_tmp());
		//str_pushf(tmp, "\\\\?\\%s", path);
		Block os_path = to_os_encoding(path_os_encoding(path));
		return DeleteFile((LPCWSTR)os_path.ptr);
	}

	bool
	file_move(const char* src, const char* dst)
	{
		Block os_src = to_os_encoding(path_os_encoding(src));
		Block os_dst = to_os_encoding(path_os_encoding(dst));
		return MoveFile((LPCWSTR)os_src.ptr, (LPCWSTR)os_dst.ptr);
	}

	bool
	folder_make(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD attributes = GetFileAttributes((LPCWSTR)os_path.ptr);
		if (attributes != INVALID_FILE_ATTRIBUTES)
			return attributes & FILE_ATTRIBUTE_DIRECTORY;
		return CreateDirectory((LPCWSTR)os_path.ptr, NULL);
	}

	bool
	folder_remove(const char* path)
	{
		Block os_path = to_os_encoding(path_os_encoding(path));
		DWORD attributes = GetFileAttributes((LPCWSTR)os_path.ptr);
		if (attributes == INVALID_FILE_ATTRIBUTES)
			return true;

		Buf<Path_Entry> files = path_entries(path, allocator_tmp());
		Str tmp_path = str_with_allocator(allocator_tmp());
		for (size_t i = 2; i < files.count; ++i)
		{
			str_clear(tmp_path);
			if (files[i].kind == Path_Entry::KIND_FILE)
			{
				tmp_path = path_join(tmp_path, path, files[i].name);
				if (file_remove(tmp_path) == false)
					return false;
			}
			else if (files[i].kind == Path_Entry::KIND_FOLDER)
			{
				tmp_path = path_join(tmp_path, path, files[i].name);
				if (folder_remove(tmp_path) == false)
					return false;
			}
			else
			{
				assert(false && "UNREACHABLE");
				return false;
			}
		}

		return RemoveDirectory((LPCWSTR)os_path.ptr);
	}

	bool
	folder_copy(const char* src, const char* dst)
	{
		Buf<Path_Entry> files = path_entries(src, allocator_tmp());

		//create the folder no matter what
		if (folder_make(dst) == false)
			return false;

		//if the source folder is empty then exit with success
		if (files.count <= 2)
			return true;

		size_t i = 0;
		Str tmp_src = str_with_allocator(allocator_tmp());
		Str tmp_dst = str_with_allocator(allocator_tmp());
		for (i = 0; i < files.count; ++i)
		{
			if(files[i].name != "." && files[i].name != "..")
			{
				str_clear(tmp_src);
				str_clear(tmp_dst);

				if (files[i].kind == Path_Entry::KIND_FILE)
				{
					tmp_src = path_join(tmp_src, src, files[i].name);
					tmp_dst = path_join(tmp_dst, dst, files[i].name);
					if (file_copy(tmp_src, tmp_dst) == false)
						break;
				}
				else if (files[i].kind == Path_Entry::KIND_FOLDER)
				{
					tmp_src = path_join(tmp_src, src, files[i].name);
					tmp_dst = path_join(tmp_dst, dst, files[i].name);
					if (folder_copy(tmp_src, tmp_dst) == false)
						break;
				}
				else
				{
					assert(false && "UNREACHABLE");
					break;
				}
			}
		}

		return i == files.count;
	}
}
#endif
