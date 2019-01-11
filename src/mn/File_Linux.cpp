#include "mn/File.h"

#if OS_LINUX

#define _LARGEFILE64_SOURCE 1
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <linux/limits.h>

//to supress warnings in release mode
#define UNUSED(x) ((void)(x))

namespace mn
{
	Block
	to_os_encoding(const Str& utf8)
	{
		return Block { utf8.ptr, utf8.count + 1 };
	}

	Block
	to_os_encoding(const char* utf8)
	{
		return to_os_encoding(str_lit(utf8));
	}

	Str
	from_os_encoding(Block os_str)
	{
		return str_lit((char*)os_str.ptr);
	}


	File
	file_stdout()
	{
		File _stdout{};
		_stdout.linux_handle = STDOUT_FILENO;
		return _stdout;
	}

	File
	file_stderr()
	{
		File _stderr{};
		_stderr.linux_handle = STDERR_FILENO;
		return _stderr;
	}

	File
	file_stdin()
	{
		File _stdin{};
		_stdin.linux_handle = STDIN_FILENO;
		return _stdin;
	}

	File
	file_open(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode)
	{
		int flags = 0;

		//translate the io mode
		switch(io_mode)
		{
			case IO_MODE::READ:
				flags |= O_RDONLY;
				break;

			case IO_MODE::WRITE:
				flags |= O_WRONLY;
				break;

			case IO_MODE::READ_WRITE:
			default:
				flags |= O_RDWR;
				break;
		}

		//translate the open mode
		switch(open_mode)
		{
			case OPEN_MODE::CREATE_ONLY:
				flags |= O_CREAT;
				flags |= O_EXCL;
				break;

			case OPEN_MODE::CREATE_APPEND:
				flags |= O_CREAT;
				flags |= O_APPEND;
				break;

			case OPEN_MODE::OPEN_ONLY:
				//do nothing
				break;

			case OPEN_MODE::OPEN_OVERWRITE:
				flags |= O_TRUNC;
				break;

			case OPEN_MODE::OPEN_APPEND:
				flags |= O_APPEND;
				break;

			case OPEN_MODE::CREATE_OVERWRITE:
			default:
				flags |= O_CREAT;
				flags |= O_TRUNC;
				break;
		}

		File result{};
		result.linux_handle = ::open(filename, flags, S_IRWXU);
		assert(result.linux_handle != -1);
		return result;
	}

	bool
	file_close(File handle)
	{
		return ::close(handle.linux_handle) == 0;
	}

	bool
	file_valid(File handle)
	{
		return handle.linux_handle != -1;
	}

	size_t
	file_write(File handle, Block data)
	{
		return ::write(handle.linux_handle, data.ptr, data.size);
	}

	size_t
	file_read(File handle, Block data)
	{
		return ::read(handle.linux_handle, data.ptr, data.size);
	}

	int64_t
	file_size(File handle)
	{
		struct stat file_stats;
		if(::fstat(handle.linux_handle, &file_stats) == 0)
		{
			return file_stats.st_size;
		}
		return -1;
	}

	int64_t
	file_cursor_pos(File handle)
	{
		off64_t offset = 0;
		return ::lseek64(handle.linux_handle, offset, SEEK_CUR);
	}

	bool
	file_cursor_move(File handle, int64_t move_offset)
	{
		off64_t offset = move_offset;
		return ::lseek64(handle.linux_handle, offset, SEEK_CUR) != -1;
	}

	bool
	file_cursor_move_to_start(File handle)
	{
		off64_t offset = 0;
		return ::lseek64(handle.linux_handle, offset, SEEK_SET) != -1;
	}

	bool
	file_cursor_move_to_end(File handle)
	{
		off64_t offset = 0;
		return ::lseek64(handle.linux_handle, offset, SEEK_END) != -1;
	}

	Str
	path_os_encoding(const char* path)
	{
		return str_from_c(path, allocator_tmp());
	}

	Str&
	path_sanitize(Str& path)
	{
		char prev = '\0';
		char *it_write = path.ptr;
		const char *it_read = path.ptr;

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
		for(char& c: path)
		{
			if(c == '\\')
				c = '/';
		}
		return path;
	}

	bool
	path_exists(const char* path)
	{
		struct stat sb{};
		return ::stat(path, &sb) == 0;
	}

	bool
	path_is_folder(const char* path)
	{
		struct stat sb{};
		if(::stat(path, &sb) == 0)
			return S_ISDIR(sb.st_mode);
		return false;
	}

	bool
	path_is_file(const char* path)
	{
		struct stat sb{};
		if(::stat(path, &sb) == 0)
			return S_ISREG(sb.st_mode);
		return false;
	}

	Str
	path_current(Allocator allocator)
	{
		char cwd[PATH_MAX] = {0};
		char* res = ::getcwd(cwd, PATH_MAX);
		return str_from_c(res, allocator);
	}

	void
	path_current_change(const char* path)
	{
		int result = ::chdir(path);
		assert(result == 0 && "chdir failed");
		UNUSED(result);
	}

	Str
	path_absolute(const char* path, Allocator allocator)
	{
		char absolute[PATH_MAX] = {0};
		char* res = ::realpath(path, absolute);
		if(res)
			return str_from_c(res, allocator);
		//here we fallback to windows-like interface and just concatencate cwd with path
		Str cwd = path_current(allocator);
		str_pushf(cwd, "/%s", path);
		return cwd;
	}

	Buf<Path_Entry>
	path_entries(const char* path, Allocator allocator)
	{
		Buf<Path_Entry> res = buf_with_allocator<Path_Entry>(allocator);

		DIR *d;
		struct dirent *dir = nullptr;
		d = ::opendir(path);
		if(d)
		{
			while((dir = ::readdir(d)) != NULL)
			{
				Path_Entry entry{};
				if(dir->d_type == DT_DIR)
				{
					entry.kind = Path_Entry::KIND_FOLDER;
				}
				else if(dir->d_type == DT_REG)
				{
					entry.kind = Path_Entry::KIND_FILE;
				}
				else
				{
					assert(false && "UNREACHABLE");
				}
				entry.name = str_from_c(dir->d_name, allocator);
				buf_push(res, entry);
			}
			::closedir(d);
		}
		return res;
	}

	bool
	file_copy(const char* src, const char* dst)
	{
		char buf[4096];
		ssize_t nread = -1;
		int fd_src = ::open(src, O_RDONLY);
		if(fd_src < 0)
			return false;
		
		int fd_dst = ::open(dst, O_WRONLY | O_CREAT | O_EXCL, 0666);
		if(fd_dst < 0)
			goto FAILURE;

		while(nread = ::read(fd_src, buf, sizeof(buf)), nread > 0)
		{
			char *out_ptr = buf;
			ssize_t nwritten = 0;
			do
			{
				nwritten = ::write(fd_dst, out_ptr, nread);
				if(nwritten >= 0)
				{
					nread -= nwritten;
					out_ptr += nwritten;
				}
				else if(errno != EINTR)
				{
					goto FAILURE;
				}
			} while(nread > 0);
		}

	FAILURE:
		::close(fd_src);
		if(fd_dst >= 0)
			::close(fd_dst);
		return nread == 0;
	}

	bool
	file_remove(const char* path)
	{
		return ::unlink(path) == 0;
	}

	bool
	file_move(const char* src, const char* dst)
	{
		return ::rename(src, dst) == 0;
	}

	bool
	folder_make(const char* path)
	{
		return ::mkdir(path, 0777) == 0;
	}

	bool
	folder_remove(const char* path)
	{
		Buf<Path_Entry> files = path_entries(path, allocator_tmp());
		Str tmp_path = str_with_allocator(allocator_tmp());
		for(size_t i = 2; i < files.count; ++i)
		{
			str_clear(tmp_path);
			if(files[i].kind == Path_Entry::KIND_FILE)
			{
				tmp_path = path_join(tmp_path, path, files[i].name);
				if(file_remove(tmp_path) == false)
					return false;
			}
			else if(files[i].kind == Path_Entry::KIND_FOLDER)
			{
				tmp_path = path_join(tmp_path, path, files[i].name);
				if(folder_remove(tmp_path) == false)
					return false;
			}
			else
			{
				assert(false && "UNREACHABLE");
				return false;
			}
		}

		return ::rmdir(path) == 0;
	}

	bool
	folder_copy(const char* src, const char* dst)
	{
		Buf<Path_Entry> files = path_entries(src, allocator_tmp());

		//create the folder no matter what
		if(folder_make(dst) == false)
			return false;

		//if the source folder is empty then exit with success
		if(files.count <= 2)
			return true;

		size_t i = 0;
		Str tmp_src = str_with_allocator(allocator_tmp());
		Str tmp_dst = str_with_allocator(allocator_tmp());
		for(i = 0; i < files.count; ++i)
		{
			if(files[i].name != "." && files[i].name != "..")
			{
				str_clear(tmp_src);
				str_clear(tmp_dst);
				if(files[i].kind == Path_Entry::KIND_FILE)
				{
					tmp_src = path_join(tmp_src, src, files[i].name);
					tmp_dst = path_join(tmp_dst, dst, files[i].name);
					if(file_copy(tmp_src, tmp_dst) == false)
						break;
				}
				else if(files[i].kind == Path_Entry::KIND_FOLDER)
				{
					tmp_src = path_join(tmp_src, src, files[i].name);
					tmp_dst = path_join(tmp_dst, dst, files[i].name);
					if(folder_copy(tmp_src, tmp_dst) == false)
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