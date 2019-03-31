#pragma once

#include "mn/Exports.h"
#include "mn/Base.h"
#include "mn/Str.h"

namespace mn
{
	/**
	 * @brief      Converts a string from utf-8 to os-specific encoding
	 *
	 * @param[in]  utf8  The utf 8
	 *
	 * @return     A Block of memory containing the encoded string
	 */
	API_MN Block
	to_os_encoding(const Str& utf8);

	/**
	 * @brief      Converts a string from utf-8 to os-specific encoding
	 *
	 * @param[in]  utf8  The utf 8
	 *
	 * @return     A Block of memory containing the encoded string
	 */
	API_MN Block
	to_os_encoding(const char* utf8);

	/**
	 * @brief      Converts from os-specific encoding to utf-8
	 *
	 * @param[in]  os_str  The operating system string
	 *
	 * @return     A String containing the encoded string
	 */
	API_MN Str
	from_os_encoding(Block os_str);


	/**
	 * @brief      OPEN_MODE enum
	 *
	 * - **CREATE_ONLY**: creates the file if it doesn't exist. if it exists it fails.
	 * - **CREATE_OVERWRITE**: creates the file if it doesn't exist. if it exists it overwrite it.
	 * - **CREATE_APPEND**: creates the file if it doesn't exist. if it exists it appends to it.
	 * - **OPEN_ONLY**: opens the file if it exists. fails otherwise.
	 * - **OPEN_OVERWRITE**: opens the file if it exist and overwrite its content. if it doesn't exist it fails.
	 * - **OPEN_APPEND**: opens the file it it exists and append to its content. if it doesn't exist it fails.
	 */
	enum class OPEN_MODE
	{
		CREATE_ONLY,
		CREATE_OVERWRITE,
		CREATE_APPEND,
		OPEN_ONLY,
		OPEN_OVERWRITE,
		OPEN_APPEND
	};

	/**
	 * @brief      IO_MODE enum
	 * 
	 * - **READ**: only performs reads to the file
	 * - **WRITE**: only performs writes to the file
	 * - **READ_WRITE**: performs both reads and writes to the file
	 */
	enum class IO_MODE
	{
		READ,
		WRITE,
		READ_WRITE
	};

	/**
	 * @brief      Represents an on disk file
	 */
	union File
	{
		void*	windows_handle;
		int32_t linux_handle;
	};

	/**
	 * @brief      Returns a file handle pointing to the standard output
	 */
	API_MN File
	file_stdout();

	/**
	 * @brief      Returns a file handle pointing to the standard error
	 */
	API_MN File
	file_stderr();

	/**
	 * @brief      Returns a file handle pointing to the standard input
	 */
	API_MN File
	file_stdin();

	/**
	 * @brief      Opens a file
	 *
	 * @param[in]  filename   The filename
	 * @param[in]  io_mode    The i/o mode
	 * @param[in]  open_mode  The open mode
	 */
	API_MN File
	file_open(const char* filename, IO_MODE io_mode, OPEN_MODE open_mode);

	/**
	 * @brief      Closes a file
	 */
	API_MN bool
	file_close(File handle);

	/**
	 * @brief      Validates a file handle
	 */
	API_MN bool
	file_valid(File handle);

	/**
	 * @brief      Writes a block of bytes into a file
	 *
	 * @param[in]  handle  The file handle
	 * @param[in]  data    The data
	 * 
	 * @return     The written size in bytes
	 */
	API_MN size_t
	file_write(File handle, Block data);

	/**
	 * @brief      Reads a block of bytes from a file
	 *
	 * @param[in]  handle  The file handle
	 * @param[in]  data    The data
	 *
	 * @return     The read size in bytes
	 */
	API_MN size_t
	file_read(File handle, Block data);

	/**
	 * @brief      Returns the size in bytes of a given file
	 */
	API_MN int64_t
	file_size(File handle);

	/**
	 * @brief      Returns the cursor position of the given file
	 */
	API_MN int64_t
	file_cursor_pos(File handle);

	/**
	 * @brief      Moves the file cursor by the given offset
	 *
	 * @param[in]  handle  The file handle
	 * @param[in]  offset  The offset
	 */
	API_MN bool
	file_cursor_move(File handle, int64_t offset);

	/**
	 * @brief      Sets the file cursor to the start
	 */
	API_MN bool
	file_cursor_move_to_start(File handle);

	/**
	 * @brief      Sets the file cursor to the end
	 */
	API_MN bool
	file_cursor_move_to_end(File handle);


	//File System api
	/**
	 * @brief      Converts from standard encoding(linux-like) to os-specific encoding
	 *
	 * @param[in]  path  The path
	 *
	 * @return     A String containing the encoded path
	 */
	API_MN Str
	path_os_encoding(const char* path);

	/**
	 * @brief      Converts from standard encoding(linux-like) to os-specific encoding
	 *
	 * @param[in]  path  The path
	 *
	 * @return     A String containing the encoded path
	 */
	inline static Str
	path_os_encoding(const Str& path)
	{
		return path_os_encoding(path.ptr);
	}

	/**
	 * Removes duplicate / and other sanitization stuff
	 */
	API_MN Str&
	path_sanitize(Str& path);

	/**
	 * Converts from the os-specific encoding to standard encoding(linux-like)
	 */
	API_MN Str&
	path_normalize(Str& path);

	inline static Str&
	path_join(Str& base)
	{
		return path_sanitize(base);
	}

	/**
	 * @brief      Joins multiple folders in the path
	 * base = "base_folder"
	 * base = path_join(base, "my_folder1", "my_folder2", "my_file") ->
	 * base_folder/my_folder1/my_folder2/my_file
	 */
	template<typename TFirst, typename ... TArgs>
	inline static Str&
	path_join(Str& base, TFirst&& first, TArgs&& ... args)
	{
		if (str_suffix(base, "/") == false)
			str_push(base, "/");
		str_push(base, std::forward<TFirst>(first));
		return path_join(base, std::forward<TArgs>(args)...);
	}

	/**
	 * @brief      Returns whether the path exists or not
	 */
	API_MN bool
	path_exists(const char* path);

	/**
	 * @brief      Returns whether the path exists or not
	 */
	inline static bool
	path_exists(const Str& path)
	{
		return path_exists(path.ptr);
	}

	/**
	 * @brief      Returns whether the given path is a folder or not
	 */
	API_MN bool
	path_is_folder(const char* path);

	/**
	 * @brief      Returns whether the given path is a folder or not
	 */
	inline static bool
	path_is_folder(const Str& path)
	{
		return path_is_folder(path.ptr);
	}

	/**
	 * @brief      Returns whether the given path is a file or not
	 */
	API_MN bool
	path_is_file(const char* path);

	/**
	 * @brief      Returns whether the given path is a file or not
	 */
	inline static bool
	path_is_file(const Str& path)
	{
		return path_is_file(path.ptr);
	}

	/**
	 * @brief      Returns the current path of the process
	 *
	 * @param[in]  allocator  The allocator to be used in the returned string
	 */
	API_MN Str
	path_current(Allocator allocator = allocator_top());

	/**
	 * @brief      Changes the current path of the process
	 *
	 * @param[in]  path  The path
	 */
	API_MN void
	path_current_change(const char* path);

	/**
	 * @brief      Changes the current path of the process
	 *
	 * @param[in]  path  The path
	 */
	inline static void
	path_current_change(const Str& path)
	{
		path_current_change(path.ptr);
	}

	/**
	 * @brief      Returns the absolute path of the given relative path
	 *
	 * @param[in]  path       The relative path
	 * @param[in]  allocator  The allocator to be used in the returns string
	 */
	API_MN Str
	path_absolute(const char* path, Allocator allocator = allocator_top());

	/**
	 * @brief      Returns the absolute path of the given relative path
	 *
	 * @param[in]  path       The relative path
	 * @param[in]  allocator  The allocator to be used in the returns string
	 */
	inline static Str
	path_absolute(const Str& path, Allocator allocator = allocator_top())
	{
		return path_absolute(path.ptr, allocator);
	}

	struct Path_Entry
	{
		enum KIND
		{
			KIND_FILE,
			KIND_FOLDER
		};

		KIND kind;
		Str  name;
	};

	inline static void
	path_entry_free(Path_Entry& self)
	{
		str_free(self.name);
	}

	inline static void
	destruct(Path_Entry& self)
	{
		path_entry_free(self);
	}

	/**
	 * @brief      Returns the names of the children files/folders of the given path
	 *
	 * @param[in]  path       The path
	 * @param[in]  allocator  The allocator to be used in the returned buf
	 */
	API_MN Buf<Path_Entry>
	path_entries(const char* path, Allocator allocator = allocator_top());

	/**
	 * @brief      Returns the names of the children files/folders of the given path
	 *
	 * @param[in]  path       The path
	 * @param[in]  allocator  The allocator to be used in the returned buf
	 */
	inline static Buf<Path_Entry>
	path_entries(const Str& path, Allocator allocator = allocator_top())
	{
		return path_entries(path.ptr, allocator);
	}

	/**
	 * @brief      Copies a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	API_MN bool
	file_copy(const char* src, const char* dst);

	/**
	 * @brief      Copies a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_copy(const Str& src, const char* dst)
	{
		return file_copy(src.ptr, dst);
	}

	/**
	 * @brief      Copies a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_copy(const char* src, const Str& dst)
	{
		return file_copy(src, dst.ptr);
	}

	/**
	 * @brief      Copies a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_copy(const Str& src, const Str& dst)
	{
		return file_copy(src.ptr, dst.ptr);
	}

	/**
	 * @brief      Removes a file
	 *
	 * @param[in]  path  The file path
	 */
	API_MN bool
	file_remove(const char* path);

	/**
	 * @brief      Removes a file
	 *
	 * @param[in]  path  The file path
	 */
	inline static bool
	file_remove(const Str& path)
	{
		return file_remove(path.ptr);
	}

	/**
	 * @brief      Moves a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	API_MN bool
	file_move(const char* src, const char* dst);

	/**
	 * @brief      Moves a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_move(const Str& src, const char* dst)
	{
		return file_move(src.ptr, dst);
	}

	/**
	 * @brief      Moves a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_move(const char* src, const Str& dst)
	{
		return file_move(src, dst.ptr);
	}

	/**
	 * @brief      Moves a file from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	file_move(const Str& src, const Str& dst)
	{
		return file_move(src.ptr, dst.ptr);
	}

	/**
	 * @brief      Creates a new folder
	 *
	 * @param[in]  path  The folder path
	 */
	API_MN bool
	folder_make(const char* path);

	/**
	 * @brief      Creates a new folder
	 *
	 * @param[in]  path  The folder path
	 */
	inline static bool
	folder_make(const Str& path)
	{
		return folder_make(path.ptr);
	}

	/**
	 * @brief      Removes a folder and the contained files/folders
	 *
	 * @param[in]  path  The folder path
	 */
	API_MN bool
	folder_remove(const char* path);

	/**
	 * @brief      Removes a folder and the contained files/folders
	 *
	 * @param[in]  path  The folder path
	 */
	inline static bool
	folder_remove(const Str& path)
	{
		return folder_remove(path.ptr);
	}

	/**
	 * @brief      Copies a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	API_MN bool
	folder_copy(const char* src, const char* dst);

	/**
	 * @brief      Copies a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_copy(const Str& src, const char* dst)
	{
		return folder_copy(src.ptr, dst);
	}

	/**
	 * @brief      Copies a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_copy(const char* src, const Str& dst)
	{
		return folder_copy(src, dst.ptr);
	}

	/**
	 * @brief      Copies a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_copy(const Str& src, const Str& dst)
	{
		return folder_copy(src.ptr, dst.ptr);
	}

	/**
	 * @brief      Moves a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_move(const char* src, const char* dst)
	{
		if (folder_copy(src, dst))
			return folder_remove(src);
		return false;
	}

	/**
	 * @brief      Moves a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_move(const Str& src, const char* dst)
	{
		return folder_move(src.ptr, dst);
	}

	/**
	 * @brief      Moves a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_move(const char* src, const Str& dst)
	{
		return folder_move(src, dst.ptr);
	}

	/**
	 * @brief      Moves a folder and the contained files/folders from src to dst
	 *
	 * @param[in]  src   The source
	 * @param[in]  dst   The destination
	 */
	inline static bool
	folder_move(const Str& src, const Str& dst)
	{
		return folder_move(src.ptr, dst.ptr);
	}
}
